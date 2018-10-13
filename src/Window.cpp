#include "Window.h"

#include "graphics/Graphics.h"

bool Window::initialized = false;

Window::Window(const char * strWindowTitle, int width, int height) {
	// We don't need any OpenGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	visible = false;

	window = glfwCreateWindow(width,height, strWindowTitle, nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

Window::~Window() {
	for (const auto &function : onDestroys)
		function(*this);

	glfwDestroyWindow(window);
}

void Window::pollEvents() {
	glfwPollEvents();
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(window);
}

bool Window::isVisible() const {
	return visible;
}

void Window::setVisible(bool _visible) {
	if (visible == _visible) return;

	visible = _visible;
	if (visible)
		glfwShowWindow(window);
	else
		glfwHideWindow(window);
}

void Window::setOnResizeListener(WindowCallback _onResize) {
	onResize = _onResize;
}

void Window::addOnDestroyListener(WindowCallback function) {
	if (function != nullptr)
		onDestroys.emplace_back(function);
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

void Window::getWindowPtr(GLFWwindow **ppWindow) {
	if (ppWindow != nullptr)
		*ppWindow = window;
}

void Window::framebufferResizeCallback(GLFWwindow * window, int width, int height) {
	auto wnd = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	if (wnd->onResize != nullptr)
		wnd->onResize(*wnd);
}
