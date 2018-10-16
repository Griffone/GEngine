/*
	Initialization vector for the executable.
*/

#include "CommonCommands.h"
#include "Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "graphics/Context.h"

#include <thread>
#include <iostream>


const int PHYSICAL_DEVICE_NAME_LENGTH = 20;

bool alive = true;
float speedModifier = 1.0f;
Window *window;
Graphics::Context *graphics = nullptr;
Graphics::Object *object = nullptr;
Graphics::Mesh *mesh = nullptr;
Graphics::Texture *texture = nullptr;
Graphics::Scene *scene = nullptr;
Graphics::Camera *camera = nullptr;

void initialize();
void cleanup();
void console();
void loadDefaults();

// Program starts here.
int main() {
	initialize();

	window = new Window("GEngine");

	// Because of reasons (Windows and Mac IO message passing) we want the window to run on main thread.
	// So I'll create a helper thread.
	std::thread thread(console);

	auto startTime = std::chrono::high_resolution_clock::now();

	while (!window->shouldClose() && alive) {
		window->pollEvents();

		if (graphics != nullptr && window->isVisible()) {
			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			object->setRotation({0.0f, time * speedModifier * 0.2f, 0.0f});

			graphics->draw(*scene);
		}
	}

	alive = false;
	window->setVisible(false);
	std::cout << "Application closing, please press enter to finish!" << std::endl;
	thread.join();

	cleanup();
	return EXIT_SUCCESS;
}


void initialize() {
	for (auto i = 0; i < sizeof(Commands::COMMON_LIST) / sizeof(Command); ++i) {
		Command cmd = Commands::COMMON_LIST[i];
		Commands::commonDict.addCommand(cmd.command, cmd.function, cmd.data);
	}
	Window::initialize();
	std::cout << "Welcome to GEngine by Griffone." << std::endl;
	std::cout << "Use \"list\" to list supported commands." << std::endl;
}

void cleanup() {
	if (camera)
		delete camera;

	if (scene)
		delete scene;

	if (object)
		delete object;

	if (mesh)
		delete mesh;

	if (texture)
		delete texture;

	if (graphics)
		delete graphics;

	if (window)
		delete window;

	Graphics::Context::terminate();
	Window::terminate();
}

void console() {
	String line;
	while (alive) {
		std::getline(std::cin, line);
		if (alive) {
			if (!Commands::processCommand(line, Commands::commonDict))
				std::cout << "Unknown command! Please use \"list\" to list supported commands." << std::endl;
		}
	}
}

void loadDefaults() {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, "data/models/cube.obj")) {
		throw std::runtime_error(err);
	}

	std::vector<Graphics::Vertex> vertices;
	std::vector<uint32_t> indices;

	std::unordered_map<Graphics::Vertex, uint32_t> uniqueVertices = {};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Graphics::Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

	mesh = new Graphics::Mesh(*graphics, vertices, indices);

	int width, height, channels;
	stbi_uc* pixels = stbi_load("data/textures/bricks.jpg", &width, &height, &channels, STBI_rgb_alpha);

	if (!pixels)
		throw std::runtime_error("Failed to load default texture!");

	texture = new Graphics::Texture(*graphics, width, height, pixels);

	object = new Graphics::Object(*mesh, *texture);

	camera = new Graphics::Camera({ 0.0f, 2.5f, -5.0f }, glm::vec3(0.0f), 45.0f);

	scene = new Graphics::Scene(*camera, *object);
	scene->lightPosition = { -2.0f, 2.0f, -3.0f };
}

void Commands::exit(String &) {
	alive = false;
}

void Commands::vulkan(String &string) {
	if (graphics == nullptr) {
		std::cout << "Initializing vulkan." << std::endl;
		Graphics::Context::initialize();
		graphics = new Graphics::Context(*window);
		loadDefaults();
	}

	if (!window->isVisible())
		window->setVisible(true);
}

void Commands::speed(String &string) {
	float newSpeed;
	if (StrUtil::parseFloat(string, &newSpeed))
		speedModifier = newSpeed;
	else
		std::cout << "Please enter a valid number!" << std::endl;
}

void Commands::load(String &) {
	std::cout << "Command is under development!" << std::endl;
}