#include "VulkanCommands.h"

#include "CommonCommands.h"

#include "graphics/Graphics.h"

#include <iostream>

CommandDictionary Commands::vulkanDict;

void Commands::vulkanList(String &) {
	auto commands = vulkanDict.getCommands();
	for (const auto &command : commands)
		std::cout << command.command << " - " << command.data.description << ".\n";
}

void Commands::vulkanHelp(String & string) {
	String word = StrUtil::firstWord(string);
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
	if (StrUtil::ltrim(string).size() == 0) {
		std::cout << "\"vulkan\" command is a forward to Vulkan commands." << std::endl;
		std::cout << "Please use \"vulkan list\" to list supported vulkan commands." << std::endl;
	} else {
		if (!Commands::processCommand(string, vulkanDict))
			std::cout << "Unknown command, please use \"vulkan list\" to list supported sub-commands." << std::endl;
	}
}

void Commands::printExtensions(String &) {
	auto extensions = Graphics::getSupportedExtensions();
	std::cout << "Available Vulkan extensions:" << std::endl;
	for (const auto &extension : extensions)
		std::cout << '\t' << extension.extensionName << " v" << StrUtil::toString(extension.specVersion) << std::endl;
}