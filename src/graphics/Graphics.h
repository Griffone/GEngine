#pragma once

/*
	A simple aliasing of Graphics API.

	Hopefully makes easier to make the rest of the code API-agnostic.
*/

#define GRAPHICS_API_VULKAN	1

#define GRAPHICS_ACTIVE_API	GRAPHICS_API_VULKAN

#if GRAPHICS_ACTIVE_API == GRAPHICS_API_VULKAN
#include "Vulkan.h"
typedef VulkanGraphics Graphics;
#endif // GRAPHICS_ACTIVE_API == GRAPHICS_API_VULKAN
