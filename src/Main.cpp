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
Graphics::Texture *normalMap = nullptr;
Graphics::Scene *scene = nullptr;
Graphics::Camera *camera = nullptr;

void initialize();
void cleanup();
void console();
void loadDefaults();
void processInput(float deltaT);


const char * const MESH_FILE = "data/models/sphere.obj";
const char * const DIFFUSE_TEXTURE_FILE = "data/textures/bricks.jpg";
const char * const NORMAL_MAP_FILE = "data/textures/bricks_norm.jpg";


// Program starts here.
int main() {
	initialize();

	window = new Window("GEngine");

	// Because of reasons (Windows and Mac IO message passing) we want the window to run on main thread.
	// So I'll create a helper thread.
	std::thread thread(console);

	while (!window->shouldClose() && alive) {
		window->pollEvents();
		
		auto lastTime = std::chrono::high_resolution_clock::now();

		if (graphics != nullptr && window->isVisible()) {
			auto currentTime = std::chrono::high_resolution_clock::now();

			float deltaT = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastTime).count();

			processInput(deltaT);

			scene->lightPosition = scene->camera.getPosition();

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

	if (normalMap)
		delete normalMap;

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

void loadMesh() {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, MESH_FILE)) {
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

	for (auto i = 0; i < indices.size(); i += 3) {
		Graphics::Vertex &v0 = vertices[indices[i + 0]];
		Graphics::Vertex &v1 = vertices[indices[i + 1]];
		Graphics::Vertex &v2 = vertices[indices[i + 2]];

		glm::vec3 deltaPos1 = v1.pos - v0.pos;
		glm::vec3 deltaPos2 = v2.pos - v0.pos;

		glm::vec2 deltaUV1 = v1.texCoord - v0.texCoord;
		glm::vec2 deltaUV2 = v2.texCoord - v0.texCoord;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = glm::normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r);
		glm::vec3 bitangent = glm::normalize((deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r);

		v2.tangent = v1.tangent = v0.tangent = tangent;
		v2.bitangent = v1.bitangent = v0.bitangent = bitangent;
	}

	mesh = new Graphics::Mesh(*graphics, vertices, indices);
}

void loadDefaults() {
	loadMesh();

	int width, height, channels;
	stbi_uc* pixels = stbi_load(DIFFUSE_TEXTURE_FILE, &width, &height, &channels, STBI_rgb_alpha);
	if (!pixels)
		throw std::runtime_error("Failed to load default texture!");

	texture = new Graphics::Texture(*graphics, width, height, pixels);


	pixels = stbi_load(NORMAL_MAP_FILE, &width, &height, &channels, STBI_rgb_alpha);
	if (!pixels)
		throw std::runtime_error("Failed to load default texture!");

	normalMap = new Graphics::Texture(*graphics, width, height, pixels);


	object = new Graphics::Object(*mesh, *texture, *normalMap);

	camera = new Graphics::Camera({ 0.0f, 2.0f, -5.0f }, glm::vec3(0.0f), 45.0f);

	scene = new Graphics::Scene(*camera, *object);
}

void processInput(float deltaT) {
	glm::vec3 fwd = scene->camera.getLookVector();
	fwd = glm::normalize(fwd);

	glm::vec3 up = { 0.0f, 1.0f, 0.0f };
	glm::vec3 right = glm::cross(fwd, up);

	// this is because currently we just rotate
	up = glm::cross(right, fwd);

	glm::vec3 delta(0.0f);

	if (window->getKey(GLFW_KEY_W) == GLFW_PRESS)
		delta.y += 1.0f;
	if (window->getKey(GLFW_KEY_S) == GLFW_PRESS)
		delta.y -= 1.0f;
	if (window->getKey(GLFW_KEY_D) == GLFW_PRESS)
		delta.x += 1.0f;
	if (window->getKey(GLFW_KEY_A) == GLFW_PRESS)
		delta.x -= 1.0f;
	if (window->getKey(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		delta.z += 1.0f;
	if (window->getKey(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		delta.z -= 1.0f;

	fwd *= delta.z;
	up *= delta.y;
	right *= delta.x;
	delta = fwd + up + right;
	camera->move(delta * deltaT * speedModifier * 4.0f);
	camera->setTarget(glm::vec3(0.0f));
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