#include "Window.h"

#include "graphics/Graphics.h"

bool Window::initialized = false;

Window::Window(const char * strWindowTitle, int width, int height) {
	glfwInit();
	// We don't need any OpenGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width,height, strWindowTitle, nullptr, nullptr);
}

Window::~Window() {
	glfwDestroyWindow(window);

	glfwTerminate();
}

std::vector<const char*> Window::getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	return extensions;
}

void Window::initialize() {
	if (!initialized)
		glfwInit();
}

void Window::terminate() {
	if (initialized)
		glfwTerminate();
}
