#include "App.h"

App::App() {
	initWindow();
}

App::~App() {
	cleanupWindow();
}

void App::loop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

void App::initWindow() {
	glfwInit();
	// We don't need any OpenGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(VULKAN_WINDOW_WIDTH, VULKAN_WINDOW_HEIGHT, VULKAN_WINDOW_TITLE, nullptr, nullptr);
}

void App::initVulkan() {}

void App::cleanupVulkan() {}

void App::cleanupWindow() {
	glfwDestroyWindow(window);

	glfwTerminate();
}
