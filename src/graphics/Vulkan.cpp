#include "Vulkan.h"

#include "../Window.h"

#include <vulkan/vulkan.h>

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

	// Getters
	std::vector<VkPhysicalDevice> getPhysicalDevices();
	VkPhysicalDeviceFeatures getDeviceFeatures(const VkPhysicalDevice &);
	VkPhysicalDeviceProperties getDeviceProperties(const VkPhysicalDevice &);

	// Make sure the Vulkan instance supports validation layers
	bool checkValidationLayerSupport();
	// Get a list of all required extensions
	// Takes into account whether validation layers are enabled or not
	std::vector<const char *> getRequiredExtensions();
	unsigned int rateDeviceSuitability(const VkPhysicalDevice &);
	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &);

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
			throw std::runtime_error("Current library supports only drawing to single window!");

		createSurface(window);
		pickPhysicalDevice();
		createLogicalDevice();

		window.addOnDestroyListener(cleanupWindow);

		lastWindow = &window;
	}

	// ====================================================================
	// ===				Vulkan private function definitions				===
	// ====================================================================

	void cleanupWindow(const Window &window) {
		if (lastWindow != &window) return;

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

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device!");

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}

	inline unsigned int rateDeviceSuitability(const VkPhysicalDevice &device) {
		// Make sure the device supports necessary queues
		QueueFamilyIndices indicies = findQueueFamilies(device);
		if (!indicies.isComplete())
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