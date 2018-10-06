/*
	Initialization vector for the executable.
*/

#include "CommonCommands.h"

#include <iostream>

bool alive = true;

void initialize();

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

	return 0;
}

void initialize() {
	for (const auto &command : Commands::COMMON_LIST)
		Commands::commonDict.addCommand(command.command, command.function, command.data);

	std::cout << "Welcome to GEngine by Griffone." << std::endl;
	std::cout << "Use \"list\" to list supported commands." << std::endl;
}

void Commands::exit(String &) {
	alive = false;
}