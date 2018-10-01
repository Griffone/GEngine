#pragma once

/*
	Console handles the terminal interaction for the project.
*/

#include "../String.h"

// Note: use of namespace can be replaced with a singleton class, but I personally prefer this syntax. It does however obstruct lifetimes of objects.
namespace Console {

	/// Initialize the console
	void initialize();

	/// Clean up any used resources
	void destruct();

	// Note: Uses object reference (&) as pointers (*) for this project signal object ownership.
	/// Parse the line and execute any commands.
	void processLine(String &string);
}