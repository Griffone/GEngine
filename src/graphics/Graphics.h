#pragma once

/*
	Currently developped app.

	The hope is to branch the engine into different areas and have a clean-ish discrimination between common and specific code.
*/

#include "String.h"

#include <vulkan/vulkan.h>

#include <vector>

/// High level collection of Graphics API couplings
class Graphics {
public:
	/// Initialize API coupling, creates very few resources
	Graphics();
	~Graphics();

	/// Get availabe physical devices
	std::vector<VkPhysicalDevice> getPhysicalDevices();

	/// Select best suited physical device for our needs
	void pickPhysicalDevice();

	/// Get Vulkan supported extensions
	static std::vector<VkExtensionProperties> getSupportedExtensions();

	// Helper functions for development
	static bool checkValidationLayerSupport();

	/// Read a physical device's properties
	static VkPhysicalDeviceProperties getDeviceProperties(const VkPhysicalDevice &);

	/// Read a physical device's features
	static VkPhysicalDeviceFeatures getDeviceFeatures(const VkPhysicalDevice &);

	/// How suitable a device is for our needs
	static int rateDeviceSuitability(const VkPhysicalDevice &);

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

#ifdef NDEBUG
	static const bool ENABLE_VALIDATION_LAYERS = false;
#else // NDEBUG
	static const bool ENABLE_VALIDATION_LAYERS = true;
#endif

private:
	// Private variables
	VkInstance instance;	// Vulkan library instance
	VkDebugUtilsMessengerEXT callback; // Callback validation layer function
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	/// Create-mesenger proxy utility function
	static VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback);
	static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator);
};