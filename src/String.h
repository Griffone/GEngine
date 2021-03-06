#pragma once

/*
	Definitions of strings for the project.

	Houses used string definition as well as helper functions.
*/

#include <string>

// All characters that get trimmed
#define STRING_WHITESPACE	"\t\n\v\f\r "

using String = std::string;

namespace StrUtil {

	template<typename Type> String toString(const Type & t) {
		return std::to_string(t);
	}

	const char *toCString(const String &);

	/// Attempt to parse a float from the beginning of a given string
	/// Returns true if succesful as well as modifying the pointed to float
	/// Returns false if the function failed for any reason
	bool parseFloat(const String &string, float *outFloat);

	/// Remove preceding or trailing whitespaces from a string
	/// Note: modifies the provided string
	String &ltrim(String &);
	String &rtrim(String &);
	String &trim(String&);

	/// Pull the first word from a string, will modify the provided string
	String firstWord(String &);

	/// Change string to lower characters, will modify the provided string
	String &lower(String &);
}