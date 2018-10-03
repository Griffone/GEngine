/*
	Initialization vector for the executable.
*/

#include "App.h"
#include "Commands.h"
#include "console/Console.h"

#include <thread>


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
		Console::processLine(line);
	}

	return EXIT_SUCCESS;
}


void initialize() {
	for (int i = 0; i < sizeof(Commands::LIST) / sizeof(Console::Command); i++) {
		Console::Command cmd = Commands::LIST[i];
		Console::Dictionary::addCommand(cmd.command, cmd.function, cmd.data);
	}
	Console::initialize();
	std::cout << "Welcome to GEngine by Griffone.\nUse \"list\" to list supported commands.\n";
}

void Commands::exit(String &) {
	alive = false;
}

void Commands::vulkan(String &) {
	// Atomic read operation
	if (app == nullptr) {
		std::thread thread = std::thread(runVulkan);
		thread.detach();
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