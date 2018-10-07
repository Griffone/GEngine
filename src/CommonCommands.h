#pragma once

/*
	Actual console-accepted commands with their data.
*/

#include "console/CommandDictionary.h"

namespace Commands {

	extern CommandDictionary commonDict;
	extern void exit(String &);
	extern void vulkan(String &);

	void commonList(String &);
	void commonHelp(String &);

	bool processCommand(String &command, const CommandDictionary &dictionary);

	const CommandData COMMON_DATA_HELP = {
		"prints help message for a specific command",
		"Usage: help <command> : prints detailed description of <command>"
	};
	const CommandData COMMON_DATA_LIST = {
		"lists all supported commands and their short descriptions.",
		"Usage: list : list all supported commands"
	};
	const CommandData COMMON_DATA_EXIT = {
		"stops the program",
		"Usage: exit : stop the program"
	};
	const CommandData COMMON_DATA_VULKAN = {
		"start the Vulkan application",
		"Usage: vulkan : start Vulkan demo application"
	};

	const Command COMMON_LIST[] = {
		{ "exit", exit, COMMON_DATA_EXIT },
		{ "list", commonList, COMMON_DATA_LIST },
		{ "help", commonHelp, COMMON_DATA_HELP},
		{ "vulkan", vulkan, COMMON_DATA_VULKAN }
	};

}