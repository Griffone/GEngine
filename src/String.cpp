#include "String.h"

#include <algorithm>

const char * StrUtil::toCString(const String & string) {
	return string.c_str();
}

String & StrUtil::ltrim(String &string) {
	string.erase(0, string.find_first_not_of(STRING_WHITESPACE));
	return string;
}

String & StrUtil::rtrim(String &string) {
	string.erase(string.find_last_not_of(STRING_WHITESPACE) + 1);
	return string;
}

String & StrUtil::trim(String &string) {
	ltrim(string);
	return rtrim(string);
}

String StrUtil::firstWord(String &string) {
	ltrim(string);
	auto it = string.find_first_of(STRING_WHITESPACE);
	String word;
	if (it == String::npos) {
		word = String(string);
		string.clear();
		return word;
	} else {
		word = string.substr(0, it);
		string.erase(0, it);
		return word;
	}
}

String & StrUtil::lower(String & string) {
	std::transform(string.begin(), string.end(), string.begin(), ::tolower);
	return string;
}
