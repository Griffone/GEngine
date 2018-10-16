#include "Window.h"

#include "graphics/Context.h"

bool Window::initialized = false;

Window::Window(const char * strWindowTitle, int width, int height) {
	// We don't need any OpenGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	visible = false;

	window = glfwCreateWindow(width,height, strWindowTitle, nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetWindowIconifyCallback(window, windowMinimizeRestoreCallback);
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
	return visible && !minimized;
}

bool Window::isMinimized() const {
	return minimized;
}

void Window::setVisible(bool _visible) {
	if (visible == _visible) return;

	visible = _visible;
	if (visible)
		glfwShowWindow(window);
	else
		glfwHideWindow(window);
}

void Window::getFramebufferSize(int * outWidth, int * outHeight) {
	glfwGetFramebufferSize(window, outWidth, outHeight);
}

void Window::addOnResizeListener(WindowCallback function) {
	if (function != nullptr)
		onResizes.emplace_back(function);
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

void Window::framebufferResizeCallback(GLFWwindow * window, int width, int height) {
	auto wnd = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	for (auto function : wnd->onResizes)
		function(*wnd);
}

void Window::windowMinimizeRestoreCallback(GLFWwindow * window, int iconified) {
	auto wnd = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	wnd->minimized = iconified == GLFW_TRUE;
}
