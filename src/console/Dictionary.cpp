#include "Dictionary.h"

#include <map>

using namespace Console;

struct CommandPayload {
	const CommandFunction	function;
	const CommandData		&data;
};

std::map<const String, const CommandPayload> commands = std::map<const String, const CommandPayload>();

void Console::Dictionary::addCommand(const std::string & command, const CommandFunction function, const CommandData &data) {
	commands.emplace(command, CommandPayload{ function, data });
}

const Command Console::Dictionary::getCommand(const std::string & command) {
	CommandPayload cp = commands.at(command);
	return Command{ command, cp.function, cp.data };
}

const std::vector<Command> Console::Dictionary::getCommands() {
	std::vector<Command> vector = std::vector<Command>();
	vector.reserve(commands.size());
	for (auto it = commands.cbegin(); it != commands.cend(); ++it)
		vector.emplace_back(Command{ it->first, it->second.function, it->second.data });
	return vector;
}

void Console::Dictionary::clear() {
	commands.clear();
}
