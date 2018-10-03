#pragma once

/*
	Currently developped app.

	The hope is to branch the engine into different areas and have a clean-ish discrimination between common and specific code.
*/

#define VULKAN_WINDOW_TITLE		"Amazing Vulkan Demo"
#define VULKAN_WINDOW_WIDTH		800
#define VULKAN_WINDOW_HEIGHT	600

#include <GLFW/glfw3.h>

/// A Vulkan app
class App {
public:
	App();
	~App();

	/// Enter the main loop
	void loop();

private:
	// Helper functions
	/// Create a GLFW window with required graphical context
	void initWindow();
	/// Initialize basic vulkan resources
	void initVulkan();

	/// Cleanup basic vulkan resources
	void cleanupVulkan();
	/// Cleanup GLFW window
	void cleanupWindow();

	// Private variables
	GLFWwindow *window;
};