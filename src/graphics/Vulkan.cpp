#include "Vulkan.h"

#include "../Window.h"

#include <vulkan/vulkan.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

namespace Vulkan {
	// ====================================================================
	// ===				Private library stucture definitions			===
	// ====================================================================
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	// ====================================================================
	// ===					Private library constants					===
	// ====================================================================
#ifdef NDEBUG
	const bool VALIDATION_LAYERS_ENABLED = false;
#else // NDEBUG
	const bool VALIDATION_LAYERS_ENABLED = true;
#endif // NDEBUG

	const std::vector<const char *> VALIDATION_LAYERS = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	const std::vector<const char *> DEVICE_EXTENSIONS = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	// ====================================================================
	// ===					Public library constants					===
	// ====================================================================

	const char * const APP_NAME = "Demo";
	const char * const ENGINE_NAME = "GEngine";
	const uint32_t APP_VERSION = VK_MAKE_VERSION(0, 0, 0);
	const uint32_t ENGINE_VERSION = VK_MAKE_VERSION(0, 0, 0);

	// ====================================================================
	// ===					Private library variables					===
	// ====================================================================
	
	bool initialized = false;

	Window *lastWindow = nullptr;


	VkInstance instance;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;

	VkSurfaceKHR surface = VK_NULL_HANDLE;
	
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;

	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;

	VkDebugUtilsMessengerEXT callback;

	// ====================================================================
	// ===	Forward declarations for private library helper functions	===
	// ====================================================================

	// Cleanup just before the window gets destroyed
	void cleanupWindow(const Window &);

	// Initialize Vulkan API context instance.
	void createVulkanInstance();
	void setupDebugCallback();

	void createSurface(Window &);
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createSwapChainImageViews();
	void createGraphicsPipeline();

	// Getters
	std::vector<VkPhysicalDevice> getPhysicalDevices();
	VkPhysicalDeviceFeatures getDeviceFeatures(const VkPhysicalDevice &);
	VkPhysicalDeviceProperties getDeviceProperties(const VkPhysicalDevice &);
	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &);
	SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice &);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &);

	// A filter for physical devices
	bool isDeviceSuitable(const VkPhysicalDevice &);

	// Make sure the Vulkan instance supports validation layers
	bool checkValidationLayerSupport();
	// Make sure given Physical Device has required extensions
	bool checkDeviceExtensionSupport(const VkPhysicalDevice &);

	// Get a list of all required extensions
	// Takes into account whether validation layers are enabled or not
	std::vector<const char *> getRequiredExtensions();
	unsigned int rateDeviceSuitability(const VkPhysicalDevice &);

	VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback);
	void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator);

	// Special case, this is a useful callback function for debugging
	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	// ====================================================================
	// ===				Vulkan public function definitions				===
	// ====================================================================

	void initialize() {
		if (initialized) return;

		lastWindow = nullptr;

		createVulkanInstance();
		setupDebugCallback();

		initialized = true;
	}

	void terminate() {
		if (!initialized) return;

		if (lastWindow != nullptr)
			cleanupWindow(*lastWindow);

		if (VALIDATION_LAYERS_ENABLED)
			destroyDebugUtilsMessengerEXT(instance, callback, nullptr);

		vkDestroyInstance(instance, nullptr);

		initialized = false;
	}

	void setupForWindow(Window &window) {
		if (lastWindow != nullptr)
			throw std::runtime_error("Current library only supports drawing to single window!");

		createSurface(window);
		pickPhysicalDevice();
		createLogicalDevice();

		window.addOnDestroyListener(cleanupWindow);

		createSwapChain();
		createSwapChainImageViews();

		lastWindow = &window;
	}

	// ====================================================================
	// ===				Vulkan private function definitions				===
	// ====================================================================

	void cleanupWindow(const Window &window) {
		if (lastWindow != &window) return;

		for (auto imageView : swapChainImageViews)
			vkDestroyImageView(device, imageView, nullptr);
		vkDestroySwapchainKHR(device, swapChain, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);

		lastWindow = nullptr;
	}

	inline void createVulkanInstance() {
		if (VALIDATION_LAYERS_ENABLED && !checkValidationLayerSupport())
			throw std::runtime_error("Validation layers requested, but not available!");

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = APP_NAME;
		appInfo.applicationVersion = APP_VERSION;
		appInfo.pEngineName = ENGINE_NAME;
		appInfo.engineVersion = ENGINE_VERSION;
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (VALIDATION_LAYERS_ENABLED) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
			createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
		} else
			createInfo.enabledLayerCount = 0;

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
			throw std::runtime_error("Failed to create instance!");
	}

	inline void setupDebugCallback() {
		if (!VALIDATION_LAYERS_ENABLED) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr; // Optional

		if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS)
			throw std::runtime_error("Failed to setup debug callback!");
	}

	inline void pickPhysicalDevice() {
		auto devices = getPhysicalDevices();

		// Use an ordered map to automatically sort candidates by increasing score
		std::multimap<unsigned int, VkPhysicalDevice> candidates;

		for (const auto& device : devices) {
			int score = rateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}

		// Check if the best candidate is suitable at all
		if (candidates.rbegin()->first > 0)
			physicalDevice = candidates.rbegin()->second;
		else
			throw std::runtime_error("Failed to find a suitable GPU!");
	}

	inline void createSurface(Window &window) {
		GLFWwindow * pWindow;
		window.getWindowPtr(&pWindow);
		if (glfwCreateWindowSurface(instance, pWindow, nullptr, &surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create a window surface!");
	}

	inline void createLogicalDevice() {
		if (device != VK_NULL_HANDLE) return;

		if (physicalDevice == VK_NULL_HANDLE) pickPhysicalDevice();

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		// Required queues for our logical device
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value() };//, indices.presentFamily.value()

		float queuePriority = 1.0f;

		for (auto queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Required logical device features
		VkPhysicalDeviceFeatures deviceFeatures = {};

		// Creation parameters for our logical device
		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = 0;

		if (VALIDATION_LAYERS_ENABLED) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
			createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
		} else
			createInfo.enabledLayerCount = 0;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());
		createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device!");

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}

	inline void createSwapChain() {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
			imageCount = swapChainSupport.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1; // Can be multiple for sterescopic projections
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // For post-processing will need to be set to something different

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		// Handle differing GPUs for drawing and presenting
		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		// TODO: handle recreation of swap-chain
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
			throw std::runtime_error("Failed to create swap chain!");

		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

		swapChainExtent = extent;
		swapChainImageFormat = surfaceFormat.format;
	}

	inline void createSwapChainImageViews() {
		swapChainImageViews.resize(swapChainImages.size());

		for (auto i = 0; i < swapChainImages.size(); ++i) {
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];

			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create image views!");
		}
	}

	inline void createGraphicsPipeline() {

	}

	bool isDeviceSuitable(const VkPhysicalDevice &device) {
		QueueFamilyIndices indices = findQueueFamilies(device);
		if (!indices.isComplete()) return false;

		if (!checkDeviceExtensionSupport(device)) return false;

		bool swapChainAdequate = false;
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

		return swapChainAdequate;
	}

	inline unsigned int rateDeviceSuitability(const VkPhysicalDevice &device) {
		if (!isDeviceSuitable(device))
			return 0;

		VkPhysicalDeviceFeatures deviceFeatures = getDeviceFeatures(device);
		// Application can't function without geometry shaders
		if (!deviceFeatures.geometryShader)
			return 0;

		int score = 0;
		VkPhysicalDeviceProperties deviceProperties = getDeviceProperties(device);

		// Discrete GPUs have a significant performance advantage
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 10000;

		// Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;


		return score;
	}

	inline std::vector<VkPhysicalDevice> getPhysicalDevices() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		return devices;
	}

	inline VkPhysicalDeviceFeatures getDeviceFeatures(const VkPhysicalDevice &device) {
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		return deviceFeatures;
	}

	inline VkPhysicalDeviceProperties getDeviceProperties(const VkPhysicalDevice &device) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		return deviceProperties;
	}

	inline QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		for (auto i = 0; !indices.isComplete(); ++i) {
			for (const auto& queueFamily : queueFamilies) {
				if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					indices.graphicsFamily = i;

				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

				if (queueFamily.queueCount > 0 && presentSupport)
					indices.presentFamily = i;
			}
		}

		return indices;
	}

	inline SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice &device) {
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}


		return details;
	}

	inline VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

		for (const auto &availableFormat : availableFormats)
			// There was some evidence in the past that BGRA is faster than RGBA, since it barely affects us - use it, we already use a verbose API
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;

		return availableFormats[0];
	}

	inline VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
		// Some drivers don't properly support FIFO PM, so we replace fallback with Immediate if it's available
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

		for (const auto& availablePresentMode : availablePresentModes) {
			// Mailbox is essentially triple buffering
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availablePresentMode;
			else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				bestMode = availablePresentMode;
		}

		return bestMode;
	}

	inline VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			VkExtent2D actualExtent = { 256, 256 };

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	inline bool checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		std::set<std::string> requiredLayers(VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());

		for (const auto &layer : availableLayers)
			requiredLayers.erase(layer.layerName);

		return requiredLayers.empty();
	}

	inline bool checkDeviceExtensionSupport(const VkPhysicalDevice &device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	inline std::vector<const char *> getRequiredExtensions() {
		std::vector<const char*> extensions = Window::getRequiredExtensions();

		if (VALIDATION_LAYERS_ENABLED)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}

	VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo, const VkAllocationCallbacks * pAllocator, VkDebugUtilsMessengerEXT * pCallback) {
		auto function = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (function != nullptr)
			return function(instance, pCreateInfo, pAllocator, pCallback);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks * pAllocator) {
		auto function = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (function != nullptr)
			function(instance, callback, pAllocator);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
		
		// Should the trigerring call be aborted?
		return VK_FALSE;
	}
}