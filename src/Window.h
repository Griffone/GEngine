#pragma once

/*
	Code relevant to handling an OS Window.
*/

#include "Global.h"

#if USE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif // USE_VULKAN

#include <GLFW/glfw3.h>

#include <vector>

#if USE_VULKAN
class Window;

namespace Vulkan {
	extern void createSurface(Window &);
}

#endif // USE_VULKAN

class Window {
#if USE_VULKAN
	friend void Vulkan::createSurface(Window &);
#endif // USE_VULKAN
public:
	typedef void(*WindowCallback)(Window &);

	Window(const char * strWindowTitle, int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);
	~Window();

	/// Poll and process basic window events
	void pollEvents();
	/// Check if the window should close
	bool shouldClose();

	/// Is the window currently visible
	bool isVisible() const;
	bool isMinimized() const;

	/// Set the visibility of the window to <param>
	void setVisible(bool);

	void getFramebufferSize(int *outWidth, int *outHeight);

	void addOnResizeListener(WindowCallback);

	/// Add a function that will be called just before window deletion
	void addOnDestroyListener(WindowCallback);
	static std::vector<const char*> getRequiredExtensions();

	static const int DEFAULT_WIDTH = 800;
	static const int DEFAULT_HEIGHT = 600;

	/// Initialize the library
	static void initialize();
	static void terminate();
private:
	static bool initialized;
	bool visible = false, minimized = false;
	GLFWwindow *window;
	std::vector<WindowCallback> onDestroys, onResizes;

	static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
	static void windowMinimizeRestoreCallback(GLFWwindow *window, int iconified);
};