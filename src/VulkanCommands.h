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
	extern void vulkanDevices(String &);

	const CommandData VULKAN_DATA_START = {
		"start a Vulkan application",
		"Starts the Vulkan interactable demo, will initialize Vulkan context if necessary.\nUsage: vulkan start : start the Vulkan demo"
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
		"Usage: vulkan extensions : print supported extensions"
	};
	const CommandData VULKAN_DATA_DEVICES = {
		"prints available devices",
		"Requires a Vulkan instance to be running\nUsage: vulkan devices : print available devices.\nPrints [device name] : [score], with larger score being more suitable for the engine."
	};

	const Command VULKAN_LIST[] = {
		{ "start", vulkanStart, VULKAN_DATA_START},
		{ "list", vulkanList, VULKAN_DATA_LIST },
		{ "help", vulkanHelp, VULKAN_DATA_HELP},
		{ "extensions", printExtensions, VULKAN_DATA_EXT },
		{ "devices", vulkanDevices, VULKAN_DATA_DEVICES }
	};
}