#pragma once

/*
	Helper structure that stores command's function location and helper string.

	Note: the structure only holds a reference to the string, as it is not always accesed.
*/

#include "../String.h"

namespace Console {

	typedef void(*CommandFunction)(String & line);

	struct CommandData {
		const String			&description;	// Short description of what the command does
		const String			&help;			// Appendix to description that provides more detailed explanation
	};

	struct Command {
		const String			&command;
		const CommandFunction	function;
		const CommandData		&data;
		//Command(const String &_command, const CommandFunction _function, const CommandData &_data) : command(_command), function(_function), data(_data) {};
	};
}