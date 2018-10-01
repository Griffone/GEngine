#include "Console.h"

#include "Dictionary.h"

#include <exception>
#include <iostream>

void Console::initialize() {}

void Console::destruct() {
	Dictionary::clear();
}

void Console::processLine(String & string) {
	String word = StringUtil::firstWord(string);
	word = StringUtil::tolower(word);

	try {
		const Command cmd = Dictionary::getCommand(word);
		cmd.function(string);
	} catch (std::exception &) {
		std::cout << '"' << word << "\" is not a legal command, please use \"list\" to list supported commands.\n";
	}
}
