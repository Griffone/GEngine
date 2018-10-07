#pragma once

/*
	Vulkan coupling.

	A single file as the rest of the program is supposed to be API-agnostic (at least in hopeful future).
*/

#include "../Window.h"

#include <cstdint>

namespace Vulkan {
	// Name of the application
	extern const char * const	APP_NAME;
	extern const char * const	ENGINE_NAME;
	extern const uint32_t		APP_VERSION;
	extern const uint32_t		ENGINE_VERSION;

	/// Initialize the Library.
	/// Must be done before any other calls to the library
	void initialize();

	/// Clanup Vulkan resources
	/// Should be the last call to the library
	void terminate();

	/// Couple to window's surface
	/// Creates the necessary pipeline for renderint images to window
	/// The pipeline will be destroyed when the window is destroyed
	void setupForWindow(Window &);
}