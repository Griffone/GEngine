#pragma once

/*
	Actual console-accepted commands with their data.
*/

#include "console/CommandDictionary.h"

namespace Commands {

	extern CommandDictionary commonDict;
	extern void exit(String &);
<<<<<<< HEAD
	extern void vulkan(String &);
=======
>>>>>>> master

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
<<<<<<< HEAD
	const CommandData COMMON_DATA_VULKAN = {
		"forward to vulkan commands",
		"Usage: vulkan <sub-command>: execute sub-command"
	};
=======
>>>>>>> master

	const Command COMMON_LIST[] = {
		{ "exit", exit, COMMON_DATA_EXIT },
		{ "list", commonList, COMMON_DATA_LIST },
<<<<<<< HEAD
		{ "help", commonHelp, COMMON_DATA_HELP},
		{ "vulkan", vulkan, COMMON_DATA_VULKAN }
=======
		{ "help", commonHelp, COMMON_DATA_HELP}
>>>>>>> master
	};

}