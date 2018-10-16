#include "Mesh.h"

#include "Context.h"

using namespace Graphics;

Graphics::Mesh::Mesh(Context & context, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indexes) : context(context), indexCount(indexes.size()) {
	//	===========================================================
	//	===					Create vertex buffer				===
	//	===========================================================
	VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	context.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(context.device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(context.device, stagingBufferMemory);

	context.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	context.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(context.device, stagingBuffer, nullptr);
	vkFreeMemory(context.device, stagingBufferMemory, nullptr);

	//	===========================================================
	//	===					Create index buffer					===
	//	===========================================================
	context.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	vkMapMemory(context.device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indexes.data(), (size_t)bufferSize);
	vkUnmapMemory(context.device, stagingBufferMemory);

	context.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
	context.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(context.device, stagingBuffer, nullptr);
	vkFreeMemory(context.device, stagingBufferMemory, nullptr);
}

Graphics::Mesh::~Mesh() {
	vkDeviceWaitIdle(context.device);

	vkDestroyBuffer(context.device, vertexBuffer, nullptr);
	vkFreeMemory(context.device, vertexBufferMemory, nullptr);

	vkDestroyBuffer(context.device, indexBuffer, nullptr);
	vkFreeMemory(context.device, indexBufferMemory, nullptr);
}
