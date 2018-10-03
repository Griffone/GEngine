#include "CommandDictionary.h"

#include <map>

CommandDictionary::CommandDictionary() {}

CommandDictionary::~CommandDictionary() {}

void CommandDictionary::addCommand(const String & command, const CommandFunction function, const CommandData & data) {
	commands.emplace(command, CommandPayload{ function, data });
}

const Command CommandDictionary::getCommand(const std::string & command) const {
	CommandPayload cp = commands.at(command);
	return Command{ command, cp.function, cp.data };
}

const std::vector<Command> CommandDictionary::getCommands() const {
	std::vector<Command> vector = std::vector<Command>();
	vector.reserve(commands.size());
	for (auto it = commands.cbegin(); it != commands.cend(); ++it)
		vector.emplace_back(Command{ it->first, it->second.function, it->second.data });
	return vector;
}

void CommandDictionary::clear() {
	commands.clear();
}