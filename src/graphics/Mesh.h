#pragma once

#include "Vertex.h"

#include <vector>

namespace Graphics {
	class Context;
	class Object;

	class Mesh {
		friend Context;
		friend Object;
	public:
		Mesh(Context &context, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
		~Mesh();

	private:
		Context &context;

		const int indexCount;

		VkBuffer		vertexBuffer, indexBuffer;
		VkDeviceMemory	vertexBufferMemory, indexBufferMemory;
	};
}