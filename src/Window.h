#pragma once

/*
	Code relevant to handling an OS Window.
*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class Window {
public:
	typedef void(*WindowCallback)(const Window &);

	Window(const char * strWindowTitle, int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);
	~Window();

	/// Poll and process basic window events
	void pollEvents();
	/// Check if the window should close
	bool shouldClose();

	/// Is the window currently visible
	bool isVisible() const;

	/// Set the visibility of the window to <param>
	void setVisible(bool);

	/// Add a function that will be called just before window deletion
	void addOnDestroyListener(WindowCallback);
	static std::vector<const char*> getRequiredExtensions();

	static const int DEFAULT_WIDTH = 800;
	static const int DEFAULT_HEIGHT = 600;

	/// Initialize the library
	static void initialize();
	static void terminate();

	// An unsafe hack around my decoupling of Graphics and OS APIs
	void getWindowPtr(GLFWwindow **);
private:
	static bool initialized;
	bool visible = false;
	GLFWwindow *window;
	std::vector<WindowCallback> onDestroy;
};