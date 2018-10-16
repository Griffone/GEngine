#pragma once

#include <vulkan/vulkan.h>

namespace Graphics {
	class Context;
	class Object;

	/*
		An unbuffered texture on the GPU
	*/
	class Texture {
		friend Context;
		friend Object;
	public:
		Texture(Context &context, int width, int height, void *data);
		~Texture();

	private:
		Context &context;

		VkImage			image;
		VkImageView		imageView;
		VkDeviceMemory	imageMemory;
		VkSampler		sampler;
	};
}