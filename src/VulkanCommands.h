#pragma once

/*
	Subset of commands for Vulkan
*/

#include "console/CommandDictionary.h"

namespace Commands {

	extern CommandDictionary vulkanDict;

	void vulkanList(String &);
	void vulkanHelp(String &);
	void printExtensions(String &);

	extern void vulkanStart(String &);

	const CommandData VULKAN_DATA_START = {
		"starts the Vulkan app",
		"Usage: vulkan start : start the app"
	};
	const CommandData VULKAN_DATA_HELP = {
		"prints help message for a specific command",
		"Usage: vulkan help <command> : prints detailed description of <command>"
	};
	const CommandData VULKAN_DATA_LIST = {
		"lists all supported commands and their short descriptions.",
		"Usage: vulkan list : list all supported sub-commands"
	};
	const CommandData VULKAN_DATA_EXT = {
		"prints supported Vulkan extensions",
		"Usage: vulkan ext : print supported extensions"
	};

	const Command VULKAN_LIST[] = {
		{ "start", vulkanStart, VULKAN_DATA_START},
		{ "list", vulkanList, VULKAN_DATA_LIST },
		{ "help", vulkanHelp, VULKAN_DATA_HELP},
		{ "ext", printExtensions, VULKAN_DATA_EXT }
	};
}