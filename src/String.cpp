#include "String.h"

#include <algorithm>

const char * StringUtil::to_cstring(const String & string) {
	return string.c_str();
}

String & StringUtil::ltrim(String &string) {
	string.erase(0, string.find_first_not_of(STRING_WHITESPACE));
	return string;
}

String & StringUtil::rtrim(String &string) {
	string.erase(string.find_last_not_of(STRING_WHITESPACE) + 1);
	return string;
}

String & StringUtil::trim(String &string) {
	ltrim(string);
	return rtrim(string);
}

String StringUtil::firstWord(String &string) {
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

String & StringUtil::tolower(String & string) {
	std::transform(string.begin(), string.end(), string.begin(), ::tolower);
	return string;
}
