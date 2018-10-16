#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <array>

namespace Graphics {
	struct Vertex {
		glm::vec3 pos;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription getBindingDescription();

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
	};
}