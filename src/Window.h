#pragma once

/*
	Code relevant to handling an OS Window.
*/

#include <GLFW/glfw3.h>

#include <vector>

class Window {
public:
	Window(const char * strWindowTitle, int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);
	~Window();

	static std::vector<const char*> getRequiredExtensions();

	static const int DEFAULT_WIDTH = 800;
	static const int DEFAULT_HEIGHT = 600;

	/// Initialize the library
	static void initialize();
	static void terminate();
private:
	static bool initialized;
	GLFWwindow *window;
};