#include "Vulkan.h"

#include "../File.h"
#include "../Window.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <algorithm>
#include <array>
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

	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;
		
		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			return attributeDescriptions;
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

	const std::vector<Vertex> VERTICES = {
		{ {-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f} },
		{ {0.5f, -0.5f}, {0.0f, 1.0f, 0.0f} },
		{ {0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} ,
		{ {-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f} }
	};

	const std::vector<uint16_t> INDICES = {
		0, 1, 2, 2, 3, 0
	};

	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
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
	bool framebufferResized = false;
	bool windowMinimized = false;

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

	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;

	VkCommandPool commandPool = VK_NULL_HANDLE;

	VkBuffer vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
	VkBuffer indexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;

	size_t currentFrame = 0;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	VkDebugUtilsMessengerEXT callback;

	// ====================================================================
	// ===	Forward declarations for private library helper functions	===
	// ====================================================================

	void cleanupWindow(Window &);
	void resizeWindow(Window &);

	// Initialize Vulkan API context instance.
	void createVulkanInstance();
	void setupDebugCallback();

	void createSurface(Window &);
	void createSwapChain(Window &);
	void createSwapChainImageViews();
	void createFramebuffers();
	void createGraphicsPipeline();
	void createRenderPass();
	void createCommandBuffers();

	// Partly reusable objects
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createCommandPool();
	void createSyncObjects();
	void createVertexBuffer();
	void createIndexBuffer();

	void cleanupSwapChain();
	void recreateSwapChain(Window &);


	// Getters
	std::vector<VkPhysicalDevice> getPhysicalDevices();
	VkPhysicalDeviceFeatures getDeviceFeatures(const VkPhysicalDevice &);
	VkPhysicalDeviceProperties getDeviceProperties(const VkPhysicalDevice &);
	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &);
	SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice &);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, Window &window);
	uint32_t findMemoryType(uint32_t typeFilter, const VkMemoryPropertyFlags &properties);

	VkShaderModule createShaderModule(const std::vector<char> &);
	void createBuffer(const VkDeviceSize &size, const VkBufferUsageFlags &usage,
		const VkMemoryPropertyFlags &properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);

	void copyBuffer(const VkBuffer &srcBuffer, const VkBuffer &dstBuffer, const VkDeviceSize &size);

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

	void draw() {
		vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain(*lastWindow);
			return;
		} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("Failed to acquire swap chain image!");

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(device, 1, &inFlightFences[currentFrame]);

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
			throw std::runtime_error("Failed to submit draw command buffer!");

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		result = vkQueuePresentKHR(presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
			recreateSwapChain(*lastWindow);
			framebufferResized = false;
		} else if (result != VK_SUCCESS)
			throw std::runtime_error("failed to present swap chain image!");

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

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

		if (indexBuffer != VK_NULL_HANDLE)
			vkDestroyBuffer(device, indexBuffer, nullptr);
		if (indexBufferMemory != VK_NULL_HANDLE)
			vkFreeMemory(device, indexBufferMemory, nullptr);

		if (vertexBuffer != VK_NULL_HANDLE)
			vkDestroyBuffer(device, vertexBuffer, nullptr);
		if (vertexBufferMemory != VK_NULL_HANDLE)
			vkFreeMemory(device, vertexBufferMemory, nullptr);

		for (auto inFlightFence : inFlightFences)
			vkDestroyFence(device, inFlightFence, nullptr);
		for (auto imageAvailableSemaphore : imageAvailableSemaphores)
			vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		for (auto renderFinishedSemaphore : renderFinishedSemaphores)
			vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);

		if (commandPool != VK_NULL_HANDLE)
			vkDestroyCommandPool(device, commandPool, nullptr);

		if (device != VK_NULL_HANDLE)
			vkDestroyDevice(device, nullptr);

		if (VALIDATION_LAYERS_ENABLED)
			destroyDebugUtilsMessengerEXT(instance, callback, nullptr);

		vkDestroyInstance(instance, nullptr);


		initialized = false;
	}

	void setupForWindow(Window &window) {
		if (lastWindow != nullptr)
			throw std::runtime_error("Current library only supports drawing to single window!");

		createSurface(window);

		if (device == VK_NULL_HANDLE) {
			pickPhysicalDevice();
			createLogicalDevice();
		}

		if (commandPool == VK_NULL_HANDLE) {
			createSyncObjects();
			createCommandPool();
			createVertexBuffer();
			createIndexBuffer();
		}

		createSwapChain(window);
		createSwapChainImageViews();
		createRenderPass();
		createGraphicsPipeline();
		createFramebuffers();
		createCommandBuffers();

		window.addOnDestroyListener(cleanupWindow);
		window.addOnResizeListener(resizeWindow);
		lastWindow = &window;
	}

	// ====================================================================
	// ===				Vulkan private function definitions				===
	// ====================================================================

	void cleanupWindow(Window &window) {
		if (lastWindow != &window) return;

		cleanupSwapChain();

		lastWindow = nullptr;
	}

	void resizeWindow(Window &) {
		framebufferResized = true;
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

	void createSurface(Window &window) {
		if (glfwCreateWindowSurface(instance, window.window, nullptr, &surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create a window surface!");
	}

	inline void createSwapChain(Window &window) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

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

	inline void createFramebuffers() {
		swapChainFramebuffers.resize(swapChainImageViews.size());

		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			VkImageView attachments[] = {
				swapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create framebuffer!");
		}
	}

	inline void createCommandBuffers() {
		commandBuffers.resize(swapChainFramebuffers.size());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate command buffers!");

		for (auto i = 0; i < commandBuffers.size(); ++i) {
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			beginInfo.pInheritanceInfo = nullptr; // Optional

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
				throw std::runtime_error("Failed to begin recording command buffer!");

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = renderPass;
			renderPassInfo.framebuffer = swapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapChainExtent;

			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			
			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

			VkBuffer vertexBuffers[] = { vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);

			vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(INDICES.size()), 1, 0, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);

			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to record command buffer!");
		}
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

	inline void createGraphicsPipeline() {
		// ========================================================================
		// ===				Start with shaders (programmable pipeline)			===
		// ========================================================================

		auto vertShaderCode = File::loadBinary("shaders/basic_vert.spv");
		auto fragShaderCode = File::loadBinary("shaders/basic_frag.spv");

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		// It's possible to use one fragment shader with different behaviors
		// Here we would specify the different entry point
		vertShaderStageInfo.pName = "main";
		// TODO: research this
		// seems like an oportunity for smart shaders with differing behavior that are optimized ahead of time
		vertShaderStageInfo.pSpecializationInfo = nullptr;

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		auto bindingDescription = Vertex::getBindingDescription();
		auto attributeDescriptions = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// ========================================================================
		// ===				Fixed Funtion part of the pipeline					===
		// ========================================================================

		// Viewport specifies which part of the frame we're drawing to
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapChainExtent.width;
		viewport.height = (float)swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		// A Scissor rectangle specifies what part of an image we're drawing
		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		// Enabling depth-clamp requires enabling GPU feature
		rasterizer.depthClampEnable = VK_FALSE;	// Clamp to 0 or 1 depth instead of discarding fragments
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;	// Modes other than fill require enabling GPU feature
		rasterizer.lineWidth = 1.0f;	// Line thickness in number of fragments, anything larger 1 requires enabling wideLines GPU feature
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		// Enabling Multi-sampling requires enabling GPU feature
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		// NOTE: VkDynamicState is a limited, but existent thing

		// In-shader layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline layout!");

		// ========================================================================
		// ===				Create the actual Pipeline object					===
		// ========================================================================

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;

		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr; // Optional
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
			throw std::runtime_error("failed to create graphics pipeline!");

		// Shader modules won't be needed later, so we release them
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
	}

	inline void createRenderPass() {
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
			throw std::runtime_error("Failed to create render pass!");
	}

	inline void createCommandPool() {
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		poolInfo.flags = 0; // Optional

		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
			throw std::runtime_error("Failed to create command pool!");
	}

	inline void createSyncObjects() {
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS
				|| vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS
				|| vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i])) {

				throw std::runtime_error("Failed to create synchronization objects for a frame!");
			}
		}
	}

	inline void createVertexBuffer() {
		VkDeviceSize bufferSize = sizeof(VERTICES[0]) * VERTICES.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, VERTICES.data(), (size_t)bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);


		copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	void createIndexBuffer() {
		VkDeviceSize bufferSize = sizeof(INDICES[0]) * INDICES.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, INDICES.data(), (size_t)bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

		copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	void cleanupSwapChain() {
		vkDeviceWaitIdle(device);

		for (auto swapChainFramebuffer : swapChainFramebuffers)
			vkDestroyFramebuffer(device, swapChainFramebuffer, nullptr);
		swapChainFramebuffers.clear();

		if (!commandBuffers.empty()) {
			vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
			commandBuffers.clear();
		}

		if (graphicsPipeline != VK_NULL_HANDLE) {
			vkDestroyPipeline(device, graphicsPipeline, nullptr);
			graphicsPipeline = VK_NULL_HANDLE;
		}

		if (pipelineLayout != VK_NULL_HANDLE) {
			vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
			pipelineLayout = VK_NULL_HANDLE;
		}

		if (renderPass != VK_NULL_HANDLE) {
			vkDestroyRenderPass(device, renderPass, nullptr);
			renderPass = VK_NULL_HANDLE;
		}

		for (auto swapChainImageView : swapChainImageViews)
			vkDestroyImageView(device, swapChainImageView, nullptr);
		swapChainImageViews.clear();

		if (swapChain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(device, swapChain, nullptr);
			swapChain = VK_NULL_HANDLE;
		}
	}

	void recreateSwapChain(Window &window) {
		vkDeviceWaitIdle(device);

		cleanupSwapChain();

		createSwapChain(window);
		createSwapChainImageViews();
		createRenderPass();
		createGraphicsPipeline();
		createFramebuffers();
		createCommandBuffers();
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

	inline VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, Window &window) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			int width, height;
			window.getFramebufferSize(&width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	uint32_t findMemoryType(uint32_t typeFilter, const VkMemoryPropertyFlags &properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (auto i = 0; i < memProperties.memoryTypeCount; ++i) {
			if ((typeFilter & (1 << i))
				&& (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {

				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type!");
	}

	inline VkShaderModule createShaderModule(const std::vector<char> &code) {
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		// Because of this line one needs to be careful with a custom allocator for code vector
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module!");

		return shaderModule;
	}

	void createBuffer(const VkDeviceSize & size, const VkBufferUsageFlags & usage, const VkMemoryPropertyFlags & properties, VkBuffer & buffer, VkDeviceMemory & bufferMemory) {
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to create buffer!");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate buffer memory!");

		vkBindBufferMemory(device, buffer, bufferMemory, 0);
	}

	void copyBuffer(const VkBuffer & srcBuffer, const VkBuffer & dstBuffer, const VkDeviceSize & size) {
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);


		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;


		vkBeginCommandBuffer(commandBuffer, &beginInfo);


		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);


		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
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