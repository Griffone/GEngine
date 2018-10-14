#pragma once

/*
	Actual console-accepted commands with their data.
*/

#include "console/CommandDictionary.h"

namespace Commands {

	extern CommandDictionary commonDict;
	extern void exit(String &);
	extern void vulkan(String &);
	extern void speed(String &);

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
		"initialize Vulkan",
		"Usage: vulkan : initialize Vulkan\nNote: will be called implicitly if other commands require Vulkan context to be initialized"
	};
	const CommandData COMMON_DATA_SPEED = {
		"change the speed of the animation",
		"Usage: speed <speed> : set the animation speed multiplier to <speed>"
	};

	const Command COMMON_LIST[] = {
		{ "exit", exit, COMMON_DATA_EXIT },
		{ "list", commonList, COMMON_DATA_LIST },
		{ "help", commonHelp, COMMON_DATA_HELP},
		{ "vulkan", vulkan, COMMON_DATA_VULKAN },
		{ "speed", speed, COMMON_DATA_SPEED }
	};

}