#pragma once

/*
	Vulkan coupling.

	A single file as the rest of the program is supposed to be API-agnostic (at least in hopeful future).
*/

#include "IGraphics.h"

#include "../File.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

class VulkanGraphics : public IGraphics {
private:
	// ========================================================================
	// ===					Private structure definitions					===
	// ========================================================================
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapchainSupportDetails {
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

	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};


public:
	// ========================================================================
	// ===								Functions							===
	// ========================================================================
	VulkanGraphics(Window &);
	~VulkanGraphics();

	void load();
	void draw();

	static void initialize();
	static void terminate();


	// ========================================================================
	// ===								Constants							===
	// ========================================================================

	static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	static const bool VALIDATION_LAYERS_ENABLED;
	static const std::vector<const char *> VALIDATION_LAYERS;
	static const std::vector<const char *> DEVICE_EXTENSIONS;
	static const std::vector<Vertex> VERTICES;
	static const std::vector<uint16_t> INDICES;
	static const char * const APP_NAME;
	static const char * const ENGINE_NAME;
	static const uint32_t APP_VERSION;;
	static const uint32_t ENGINE_VERSION;


private:
	// ========================================================================
	// ===							Variables								===
	// ========================================================================

	static bool initialized;
	static VkInstance instance;
	static VkDebugUtilsMessengerEXT callback;

	size_t							currentFrame;
	
	VkPhysicalDevice				physicalDevice;
	VkDevice						device;
	
	VkSurfaceKHR					surface;
	
	VkQueue							graphicsQueue;
	VkQueue							presentQueue;
	
	VkSwapchainKHR					swapchain;
	VkFormat						swapchainImageFormat;
	VkExtent2D						swapchainExtent;
	std::vector<VkImage>			swapchainImages;
	std::vector<VkImageView>		swapchainImageViews;
	std::vector<VkFramebuffer>		swapchainFramebuffers;

	VkRenderPass					renderPass;
	VkDescriptorSetLayout			descriptorSetLayout;
	VkPipelineLayout				pipelineLayout;
	VkPipeline						graphicsPipeline;

	VkCommandPool					commandPool;
	std::vector<VkCommandBuffer>	commandBuffers;

	VkBuffer						vertexBuffer, indexBuffer;
	VkDeviceMemory					vertexBufferMemory, indexBufferMemory;

	std::vector<VkSemaphore>		imageAvailableSemaphores, renderFinishedSemaphores;
	std::vector<VkFence>			inFlightFences;


	// ========================================================================
	// ===							Helper functions						===
	// ========================================================================

	static void createVulkanInstance();
	static void setupDebugCallback();


	static VkResult createDebugUtilsMessengerEXT(VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pCallback);
	static void destroyDebugUtilsMessengerEXT(VkInstance instance,
		VkDebugUtilsMessengerEXT callback,
		const VkAllocationCallbacks* pAllocator);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
	
	
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	
	void createCommandPool();
	void createVertexBuffer();
	void createIndexBuffer();
	void createCommandBuffers();
	void createSyncObjects();

	void createSwapchain();
	void createImageViews();
	void createRenderPass();
	void createGraphicsPipeline();
	void createFramebuffers();


	void cleanupSwapchain();
	void cleanup();

	void recreateSwapchain();


	static std::vector<VkPhysicalDevice> getPhysicalDevices();
	static VkPhysicalDeviceFeatures getDeviceFeatures(const VkPhysicalDevice &);
	static VkPhysicalDeviceProperties getDeviceProperties(const VkPhysicalDevice &);
	static std::vector<const char *> getRequiredExtensions();
	static bool checkValidationLayerSupport();
	static bool checkDeviceExtensionSupport(const VkPhysicalDevice &);

	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &);
	SwapchainSupportDetails querySwapchainSupport(const VkPhysicalDevice &);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
	uint32_t findMemoryType(uint32_t typeFilter, const VkMemoryPropertyFlags &properties);

	VkShaderModule createShaderModule(const std::vector<char> &);
	void createBuffer(const VkDeviceSize &size, const VkBufferUsageFlags &usage,
		const VkMemoryPropertyFlags &properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);

	void copyBuffer(const VkBuffer &srcBuffer, const VkBuffer &dstBuffer, const VkDeviceSize &size);

	unsigned int rateDeviceSuitability(const VkPhysicalDevice &);
	bool isDeviceSuitable(const VkPhysicalDevice &);

};