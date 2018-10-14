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
Window *window;
IGraphics *graphics = nullptr;

void initialize();
void cleanup();
void console();

// Program starts here.
int main() {
	initialize();

	window = new Window("GEngine");

	// Because of reasons (Windows and Mac IO message passing) we want the window to run on main thread.
	// So I'll create a helper thread.
	std::thread thread(console);

	while (!window->shouldClose()) {
		window->pollEvents();

		if (graphics != nullptr && window->isVisible()) {
			// TODO: Draw stuff
			graphics->draw();
		}
	}

	alive = false;
	window->setVisible(false);
	std::cout << "Application closing, please press enter to finish!" << std::endl;
	thread.join();

	cleanup();
	return EXIT_SUCCESS;
}


void initialize() {
	for (auto i = 0; i < sizeof(Commands::COMMON_LIST) / sizeof(Command); ++i) {
		Command cmd = Commands::COMMON_LIST[i];
		Commands::commonDict.addCommand(cmd.command, cmd.function, cmd.data);
	}
	Window::initialize();
	std::cout << "Welcome to GEngine by Griffone." << std::endl;
	std::cout << "Use \"list\" to list supported commands." << std::endl;
}

void cleanup() {
	if (graphics)
		delete graphics;

	if (window)
		delete window;

	Graphics::terminate();
	Window::terminate();
}

void console() {
	String line;
	while (alive) {
		std::getline(std::cin, line);
		if (alive) {
			if (!Commands::processCommand(line, Commands::commonDict))
				std::cout << "Unknown command! Please use \"list\" to list supported commands." << std::endl;
		}
	}
}

void Commands::exit(String &) {
	alive = false;
}

void Commands::vulkan(String &string) {
	if (graphics == nullptr) {
		std::cout << "Initializing vulkan." << std::endl;
		Graphics::initialize();
		graphics = new Graphics(*window);
	}

	if (!window->isVisible())
		window->setVisible(true);
}