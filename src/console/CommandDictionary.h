#pragma once

/*
	Console command dictionary.

	A helper structure that stores commands, their related functions and help strings.
*/

#include "Command.h"

#include <map>
#include <vector>

struct CommandPayload {
	const CommandFunction	function;
	const CommandData		&data;
};

class CommandDictionary {
public:
	CommandDictionary();
	~CommandDictionary();

	/// Add a command to internal dictionary
	void addCommand(const String &command, const CommandFunction function, const CommandData &data);

	/// Attempt to get a command from internal dictionary
	/// Throws an exception if a command is not found
	const Command getCommand(const std::string &) const;

	/// Get an array of all commands in the dictionary
	const std::vector<Command> getCommands() const;

	/// Clear internal dictionary
	void clear();

private:
	std::map<const String, const CommandPayload> commands;
};