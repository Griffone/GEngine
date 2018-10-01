#pragma once

/*
	Actual console-accepted commands with their data.
*/

#include "console/Dictionary.h"

#include <iostream>

namespace Commands {

	extern void exit(String &);

	void listCommands(String &);
	void help(String &);

	const Console::CommandData DATA_HELP = {
		"prints help message for a specific command",
		"Usage: help <command> : prints detailed description of <command>"
	};
	const Console::CommandData DATA_LIST = {
		"lists all supported commands and their short descriptions.",
		"Usage: list : list all supported commands"
	};
	const Console::CommandData DATA_EXIT = {
		"stops the program",
		"Usage: exit : stop the program"
	};

	const Console::Command LIST[] = {
		{ "exit", exit, DATA_EXIT },
		{ "list", listCommands, DATA_LIST },
		{ "help", help, DATA_HELP}
	};

	void listCommands(String &) {
		auto commands = Console::Dictionary::getCommands();
		for (auto it = commands.begin(); it != commands.end(); ++it)
			std::cout << it->command << " - " << it->data.description << ".\n";
	}

	void help(String & string) {
		String word = StringUtil::firstWord(string);
		if (word.empty())
			word = "help";
		try {
			const Console::Command command = Console::Dictionary::getCommand(word);
			std::cout << command.command << " - " << command.data.description << ".\n" << command.data.help << '\n';
		} catch (std::exception &e) {
			std::cout << "Unknown command \"" << word << "\", please use \"list\" to list supported commands.\n";
		}
	}
}