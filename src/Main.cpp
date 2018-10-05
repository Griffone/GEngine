/*
	Initialization vector for the executable.
*/

#include "App.h"
#include "CommonCommands.h"
#include "VulkanCommands.h"

#include <thread>
#include <iostream>


bool alive = true;
App *app = nullptr;

void initialize();
void runVulkan();

// Program starts here.
int main() {
	initialize();

	String line;
	while (alive) {
		std::getline(std::cin, line);
		if (!Commands::processCommand(line, Commands::commonDict))
			std::cout << "Unknown command, please use \"list\" to list supported commands.\n";
	}

	return EXIT_SUCCESS;
}


void initialize() {
	for (auto i = 0; i < sizeof(Commands::COMMON_LIST) / sizeof(Command); ++i) {
		Command cmd = Commands::COMMON_LIST[i];
		Commands::commonDict.addCommand(cmd.command, cmd.function, cmd.data);
	}
	for (auto i = 0; i < sizeof(Commands::VULKAN_LIST) / sizeof(Command); ++i) {
		Command cmd = Commands::VULKAN_LIST[i];
		Commands::vulkanDict.addCommand(cmd.command, cmd.function, cmd.data);
	}
	std::cout << "Welcome to GEngine by Griffone." << std::endl;
	std::cout << "Use \"list\" to list supported commands." << std::endl;
}

void Commands::exit(String &) {
	alive = false;
}

void Commands::vulkanStart(String &) {
	// Atomic read operation, so no special adressing
	if (app == nullptr) {
		std::thread thread(runVulkan);
		std::cout << "Running Vulkan." << std::endl;
		thread.detach();
	} else {
		std::cout << "A Vulkan instance is already running!" << std::endl;
	}
}

void runVulkan() {
	app = new App();
	try {
		app->loop();
		std::cout << "vulkan app stopped" << std::endl;
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	delete app;
}