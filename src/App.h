#pragma once

/*
	Currently developped app.

	The hope is to branch the engine into different areas and have a clean-ish discrimination between common and specific code.
*/

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <vector>

namespace Vulkan {
	const int DEFAULT_WIDTH =					800;
	const int DEFAULT_HEIGHT =					600;

	const char * const APP_NAME =				"Demo";
	const char * const ENGINE_NAME =			"GEngine";
	const char * const DEFAULT_WINDOW_TITLE =	"Vulkan Demo";

	const uint32_t APP_VERSION =				VK_MAKE_VERSION(0, 0, 0);
	const uint32_t ENGINE_VERSION =				VK_MAKE_VERSION(0, 0, 0);

	const std::vector<const char *> VALIDATION_LAYERS = {
			"VK_LAYER_LUNARG_standard_validation"
	};

#ifdef NDEBUG
	const bool ENABLE_VALIDATION_LAYERS = false;
#else // NDEBUG
	const bool ENABLE_VALIDATION_LAYERS = true;
#endif
}

/// A Vulkan app
class App {
public:
	App();
	~App();

	/// Enter the main loop
	void loop();

	/// Get Vulkan supported extensions
	static std::vector<VkExtensionProperties> getSupportedExtensions();

	// Helper functions for development
	static bool checkValidationLayerSupport();
	static std::vector<const char *> getRequiredExtensions();

	// Debug callback function
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);

private:
	// Helper functions
	/// Create a GLFW window with required graphical context
	void initWindow();
	/// Initialize basic vulkan resources
	void initVulkan();
	/// Initialize vulkan validation layers
	void setupDebugCallback();

	/// Cleanup basic vulkan resources
	void cleanupVulkan();
	/// Cleanup GLFW window
	void cleanupWindow();

	/// Create-mesenger proxy utility function
	static VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback);
	static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator);

	// Private variables
	GLFWwindow *window;
	VkInstance instance;	// Vulkan library instance
	VkDebugUtilsMessengerEXT callback; // Callback validation layer function
};