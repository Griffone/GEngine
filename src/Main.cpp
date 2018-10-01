/*
	Initialization vector for the executable.
*/

#include "Commands.h"
#include "console/Console.h"

bool alive = true;

void initialize();

// Program starts here.
int main() {
	initialize();

	String line;
	while (alive) {
		std::getline(std::cin, line);
		Console::processLine(line);
	}

	return 0;
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