#pragma once

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <vulkan/vulkan.h>

#include <array>

namespace Graphics {
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCoord;

		glm::vec3 tangent;
		glm::vec3 bitangent;

		static VkVertexInputBindingDescription getBindingDescription();

		static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions();

		bool operator==(const Vertex &) const;
	};
}

namespace std {
	template<> struct hash<Graphics::Vertex> {
		size_t operator()(Graphics::Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}