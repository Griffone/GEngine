#include "VulkanCommands.h"

#include "CommonCommands.h"

#include <iostream>

CommandDictionary Commands::vulkanDict;

void Commands::vulkanList(String &) {
	auto commands = vulkanDict.getCommands();
	for (auto it = commands.begin(); it != commands.end(); ++it)
		std::cout << it->command << " - " << it->data.description << ".\n";
}

void Commands::vulkanHelp(String & string) {
	String word = StringUtil::firstWord(string);
	if (word.empty())
		word = "help";
	try {
		const Command command = vulkanDict.getCommand(word);
		std::cout << command.command << " - " << command.data.description << '.' << std::endl << command.data.help << std::endl;
	} catch (std::exception &e) {
		std::cout << "Unknown command \"" << word << "\", please use \"vulkan list\" to list supported sub-commands." << std::endl;
	}
}

void Commands::vulkan(String & string) {
	if (!Commands::processCommand(string, vulkanDict))
		std::cout << "Unknown command, please use \"vulkan list\" to list supported sub-commands." << std::endl;
}