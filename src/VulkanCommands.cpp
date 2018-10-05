#include "VulkanCommands.h"

#include "App.h"
#include "CommonCommands.h"

#include <iostream>

CommandDictionary Commands::vulkanDict;

void Commands::vulkanList(String &) {
	auto commands = vulkanDict.getCommands();
	for (const auto &command : commands)
		std::cout << command.command << " - " << command.data.description << ".\n";
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

void Commands::printExtensions(String &) {
	auto extensions = App::getSupportedExtensions();
	std::cout << "Available Vulkan extensions:" << std::endl;
	for (const auto &extension : extensions)
		std::cout << '\t' << extension.extensionName << " v" << std::to_string(extension.specVersion) << std::endl;
}