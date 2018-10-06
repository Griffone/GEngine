#pragma once

/*
	Driver-specific code dealing with hardware-accelerated rendering.
*/

#include "String.h"
#include "../Window.h"

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

/// High level collection of Graphics API couplings
class Graphics {
public:
	/// Initialize API coupling, creates very few resources
	Graphics();
	~Graphics();

	/// Get availabe physical devices
	std::vector<VkPhysicalDevice> getPhysicalDevices();

	/// How suitable a device is for our needs
	int rateDeviceSuitability(const VkPhysicalDevice &);

	/// Select best suited physical device for our needs
	void pickPhysicalDevice();

	/// Set up a logical device linked to picked physical device
	void createLogicalDevice();

	/// Create a draw surface on a given window
	void createDrawSurface(Window &);

	/// Get Vulkan supported extensions
	static std::vector<VkExtensionProperties> getSupportedExtensions();

	// Helper functions for development
	static bool checkValidationLayerSupport();
	/// Make sure the device supports required extensions
	static bool checkDeviceExtensionsSupport(const VkPhysicalDevice &);

	/// Read a physical device's properties
	static VkPhysicalDeviceProperties getDeviceProperties(const VkPhysicalDevice &);

	/// Read a physical device's features
	static VkPhysicalDeviceFeatures getDeviceFeatures(const VkPhysicalDevice &);

	/// Read a physical device's supported features
	static std::vector<VkExtensionProperties> getDeviceSupportedExtensions(const VkPhysicalDevice &);


	// Debug callback function
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);

	static const char * const APP_NAME;
	static const char * const ENGINE_NAME;

	static const uint32_t APP_VERSION = VK_MAKE_VERSION(0, 0, 0);
	static const uint32_t ENGINE_VERSION = VK_MAKE_VERSION(0, 0, 0);

	static const std::vector<const char *> VALIDATION_LAYERS;
	static const std::vector<const char *> DEVICE_EXTENSIONS;

#ifdef NDEBUG
	static const bool ENABLE_VALIDATION_LAYERS = false;
#else // NDEBUG
	static const bool ENABLE_VALIDATION_LAYERS = true;
#endif

private:
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

	// Initialized private variables
	VkInstance					instance;	// Vulkan library instance
	VkDebugUtilsMessengerEXT	callback;	// Callback validation layer function
	VkQueue						graphicsQueue;

	// Uninitialized private varibales, requiring calls to functions after creation to work
	VkPhysicalDevice			physicalDevice = VK_NULL_HANDLE;
	VkDevice					device = VK_NULL_HANDLE;	// Logical device
	VkSurfaceKHR				surface = VK_NULL_HANDLE;	// A window surface
	VkQueue						presentQueue;

	/// Create-mesenger proxy utility function
	static VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback);
	static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator);

	/// Look for all necessary queue families
	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &);

	SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice &);
};