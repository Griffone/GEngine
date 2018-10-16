#include "File.h"

std::vector<char> File::loadBinary(const String &name) {
	// Start reading file at the end, to know its size
	std::ifstream file(name, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		throw FileException("Failed to open file!");

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

File::FileException::FileException(const String &reason) : reason(reason) {}

const char * File::FileException::what() const throw() {
	return StrUtil::toCString(reason);
}
