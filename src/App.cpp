#include "App.h"

#include <stdexcept>
#include <vector>

App::App() {
	initWindow();
	initVulkan();
}

App::~App() {
	cleanupVulkan();
	cleanupWindow();
}

void App::loop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

std::vector<VkExtensionProperties> App::getSupportedExtensions() const {
	uint32_t extensionCount = 0;
	// Get instance count
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	// Allocate an array for data
	std::vector<VkExtensionProperties> extensions(extensionCount);
	// Get instance data
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	return extensions;
}

void App::initWindow() {
	glfwInit();
	// We don't need any OpenGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(VULKAN_WINDOW_WIDTH, VULKAN_WINDOW_HEIGHT, VULKAN_WINDOW_TITLE, nullptr, nullptr);
}

void App::initVulkan() {
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	appInfo.pApplicationName = VULKAN_APP_NAME;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

	appInfo.pEngineName = VULKAN_ENGINE_NAME;
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

	appInfo.apiVersion = VK_API_VERSION_1_0;


	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		throw std::runtime_error("failed to create instance!");
}

void App::cleanupVulkan() {
	vkDestroyInstance(instance, nullptr);
}

void App::cleanupWindow() {
	glfwDestroyWindow(window);

	glfwTerminate();
}
