#include "CommonCommands.h"

#include <iostream>

CommandDictionary Commands::commonDict;

void Commands::commonList(String &) {
	auto commands = commonDict.getCommands();
	for (const auto &command : commands)
		std::cout << command.command << " - " << command.data.description << ".\n";
}

void Commands::commonHelp(String & string) {
	String word = StrUtil::firstWord(string);
	if (word.empty())
		word = "help";
	try {
		const Command command = commonDict.getCommand(word);
		std::cout << command.command << " - " << command.data.description << ".\n" << command.data.help << '\n';
	} catch (CommandNotFoundException &e) {
		std::cout << "Unknown command \"" << word << "\", please use \"list\" to list supported commands.\n";
	}
}

bool Commands::processCommand(String & line, const CommandDictionary & dictionary) {
	String word = StrUtil::firstWord(line);
	word = StrUtil::lower(word);
	try {
		const Command command = dictionary.getCommand(word);
		command.function(line);
	} catch (CommandNotFoundException &e) {
		return false;
	}
	return true;
}
