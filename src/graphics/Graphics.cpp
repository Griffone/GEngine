#include "Graphics.h"

#include "../Window.h"

#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

const char * const Graphics::APP_NAME = "Demo";
const char * const Graphics::ENGINE_NAME = "GEngine";

const std::vector<const char *> Graphics::VALIDATION_LAYERS = {
		   "VK_LAYER_LUNARG_standard_validation"
};

Graphics::Graphics() {
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

	auto extensions = Window::getRequiredExtensions();
	if (ENABLE_VALIDATION_LAYERS)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (ENABLE_VALIDATION_LAYERS) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
		createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		throw std::runtime_error("failed to create instance!");
	
	// Set up debug function callback
	if (ENABLE_VALIDATION_LAYERS) {
		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr; // Optional

		if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS)
			throw std::runtime_error("failed to set up debug callback!");
	}

	std::cout << "Graphics succesfully initialized" << std::endl;
}

Graphics::~Graphics() {
	if (ENABLE_VALIDATION_LAYERS)
		destroyDebugUtilsMessengerEXT(instance, callback, nullptr);

	vkDestroyInstance(instance, nullptr);
}

std::vector<VkPhysicalDevice> Graphics::getPhysicalDevices() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	return devices;
}

std::vector<VkExtensionProperties> Graphics::getSupportedExtensions() {
	uint32_t extensionCount = 0;
	// Get instance count
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	// Allocate an array for data
	std::vector<VkExtensionProperties> extensions(extensionCount);
	// Get instance data
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	return extensions;
}

bool Graphics::checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char *layerName : VALIDATION_LAYERS) {
		bool layerFound = false;

		for (const auto &layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
			return false;
	}

	return true;
}

VkPhysicalDeviceProperties Graphics::getDeviceProperties(const VkPhysicalDevice &device) {
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	return deviceProperties;
}

VkPhysicalDeviceFeatures Graphics::getDeviceFeatures(const VkPhysicalDevice &device) {
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	return deviceFeatures;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Graphics::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData, void * pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

void Graphics::pickPhysicalDevice() {
	auto devices = getPhysicalDevices();

	// Use an ordered map to automatically sort candidates by increasing score
	std::multimap<int, VkPhysicalDevice> candidates;

	for (const auto& device : devices) {
		int score = rateDeviceSuitability(device);
		candidates.insert(std::make_pair(score, device));
	}

	// Check if the best candidate is suitable at all
	if (candidates.rbegin()->first > 0)
		physicalDevice = candidates.rbegin()->second;
	else
		throw std::runtime_error("failed to find a suitable GPU!");
}

int Graphics::rateDeviceSuitability(const VkPhysicalDevice & device) {
	VkPhysicalDeviceProperties deviceProperties = getDeviceProperties(device);
	VkPhysicalDeviceFeatures deviceFeatures = getDeviceFeatures(device);
	
	int score = 0;

	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;

	// Application can't function without geometry shaders
	if (!deviceFeatures.geometryShader) {
		return 0;
	}

	return score;
}

VkResult Graphics::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo, const VkAllocationCallbacks * pAllocator, VkDebugUtilsMessengerEXT * pCallback) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pCallback);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void Graphics::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks * pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func)
		func(instance, callback, pAllocator);
}