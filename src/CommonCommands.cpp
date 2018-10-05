#include "CommonCommands.h"

#include <iostream>

CommandDictionary Commands::commonDict;

void Commands::commonList(String &) {
	auto commands = commonDict.getCommands();
	for (const auto &command : commands)
		std::cout << command.command << " - " << command.data.description << ".\n";
}

void Commands::commonHelp(String & string) {
	String word = StringUtil::firstWord(string);
	if (word.empty())
		word = "help";
	try {
		const Command command = commonDict.getCommand(word);
		std::cout << command.command << " - " << command.data.description << ".\n" << command.data.help << '\n';
	} catch (std::exception &e) {
		std::cout << "Unknown command \"" << word << "\", please use \"list\" to list supported commands.\n";
	}
}

bool Commands::processCommand(String & line, const CommandDictionary & dictionary) {
	String word = StringUtil::firstWord(line);
	word = StringUtil::tolower(word);
	try {
		const Command command = dictionary.getCommand(word);
		command.function(line);
	} catch (std::exception &e) {
		return false;
	}
	return true;
}
