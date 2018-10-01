#pragma once

/*
	Console command dictionary.

	A helper structure that stores commands, their related functions and help strings.
*/

#include "Command.h"

#include <vector>

namespace Console {

	namespace Dictionary {

		/// Add a command to internal dictionary
		void addCommand(const String &command, const CommandFunction function, const CommandData &data);

		/// Attempt to get a command from internal dictionary
		/// Throws an exception if a command is not found
		const Command getCommand(const std::string &);

		/// Get an array of all commands in the dictionary
		const std::vector<Command> getCommands();

		/// Clear internal dictionary
		void clear();
	}
}