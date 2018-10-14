#include "CommandDictionary.h"

#include <map>

CommandDictionary::CommandDictionary() {}

CommandDictionary::~CommandDictionary() {}

void CommandDictionary::addCommand(const String & command, const CommandFunction function, const CommandData & data) {
	commands.emplace(command, CommandPayload{ function, data });
}

const Command CommandDictionary::getCommand(const std::string & command) const {
	try {
		CommandPayload cp = commands.at(command);
		return Command{ command, cp.function, cp.data };
	} catch (const std::exception &e) {
		throw CommandNotFoundException();
	}
}

const std::vector<Command> CommandDictionary::getCommands() const {
	std::vector<Command> vector = std::vector<Command>();
	vector.reserve(commands.size());
	for (const auto &command : commands)
		vector.emplace_back(Command{command.first, command.second.function, command.second.data});
	return vector;
}

void CommandDictionary::clear() {
	commands.clear();
}
