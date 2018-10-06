/*
	Initialization vector for the executable.
*/

#include "CommonCommands.h"
#include "VulkanCommands.h"
#include "Window.h"

#include "graphics/Graphics.h"

#include <thread>
#include <iostream>


const int PHYSICAL_DEVICE_NAME_LENGTH = 20;

bool alive = true;
Window *window = nullptr;
Graphics *graphics = nullptr;

void initialize();
void cleanup();

// Program starts here.
int main() {
	initialize();

	String line;
	while (alive) {
		std::getline(std::cin, line);
		if (!Commands::processCommand(line, Commands::commonDict))
			std::cout << "Unknown command, please use \"list\" to list supported commands.\n";
	}

	cleanup();
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

void cleanup() {
	if (window)
		delete window;

	if (graphics)
		delete graphics;
}

void Commands::exit(String &) {
	alive = false;
}

void Commands::vulkanStart(String &) {
	if (graphics == nullptr)
		graphics = new Graphics();

	// Atomic read operation, so no special adressing
	if (window == nullptr) {
		std::cout << "The command is currently under development..." << std::endl;
	} else
		std::cout << "The Vulkan app is already running!" << std::endl;
}

void Commands::vulkanDevices(String & string) {
	if (graphics == nullptr)
		graphics = new Graphics();

	for (const auto & device : graphics->getPhysicalDevices()) {
		auto deviceProperties = Graphics::getDeviceProperties(device);
		printf("%-*s : %i", PHYSICAL_DEVICE_NAME_LENGTH, deviceProperties.deviceName, graphics->rateDeviceSuitability(device));
		std::cout << std::endl;
	}
}