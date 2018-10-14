#pragma once

/*
	General purpose file loading.

	Will likely be expanded to account for different formats in the future
*/

#include "String.h"

#include <fstream>
#include <vector>

namespace File {

	/// Read a given file as a binary.
	/// Will allocate a vector exactly the size of the file and read file data into it.
	/// Does no processing of the file.
	/// <throws> "Failed to open file" runtime error </throws>
	std::vector<char> loadBinary(const String &);
}