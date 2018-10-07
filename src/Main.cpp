/*
	Initialization vector for the executable.
*/

#include "CommonCommands.h"
#include "Window.h"

#include "graphics/Graphics.h"

#include <thread>
#include <iostream>


const int PHYSICAL_DEVICE_NAME_LENGTH = 20;

bool alive = true;
Window *window = nullptr;

void initialize();
void cleanup();

// Program starts here.
int main() {
	initialize();

	String line;
	while (alive) {
		std::getline(std::cin, line);
		if (!Commands::processCommand(line, Commands::commonDict)) {
			std::cout << "Unknown command! Please use \"list\" to list supported commands." << std::endl;
		}
	}

	cleanup();
	return EXIT_SUCCESS;
}


void initialize() {
	for (auto i = 0; i < sizeof(Commands::COMMON_LIST) / sizeof(Command); ++i) {
		Command cmd = Commands::COMMON_LIST[i];
		Commands::commonDict.addCommand(cmd.command, cmd.function, cmd.data);
	}
	std::cout << "Welcome to GEngine by Griffone." << std::endl;
	std::cout << "Use \"list\" to list supported commands." << std::endl;
}

void cleanup() {
	if (window)
		delete window;

	Graphics::terminate();
	Window::terminate();
}

void Commands::exit(String &) {
	alive = false;
}

void windowLife() {
	window = new Window("Vulkan Demo");
	Graphics::setupForWindow(*window);

	while (!window->shouldClose()) {
		window->pollEvents();

		// Draw something
	}

	delete window;
	window = nullptr;
}

void Commands::vulkan(String &string) {
	if (window) {
		std::cout << "A Vulkan app is already running." << std::endl;
		return;
	}

	Window::initialize();
	Graphics::initialize();
	std::thread thread(windowLife);
	thread.detach();
}