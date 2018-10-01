#pragma once

/*
	Definitions of strings for the project.

	Houses used string definition as well as helper functions.
*/

#include <string>

// All characters that get trimmed
#define STRING_WHITESPACE	"\t\n\v\f\r "

using String = std::string;

namespace StringUtil {

	const char *to_cstring(const String &);

	/// Remove preceding or trailing whitespaces from a string
	/// Note: modifies the provided string
	String &ltrim(String &);
	String &rtrim(String &);
	String &trim(String&);

	/// Pull the first word from a string, will modify the provided string
	String firstWord(String &);

	/// Change string to lower characters, will modify the provided string
	String &tolower(String &);
}