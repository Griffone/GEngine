/*
	Initialization vector for the executable.
*/

#include "CommonCommands.h"
#include "Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

			object->setRotation({0.0f, time * speedModifier, 0.0f});

			graphics->draw(*object);
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
	std::vector<Graphics::Vertex> verts = {
		{{-0.5f, 0.5f, -0.5f},	{1.0f, 0.0f}},
		{{0.5f, 0.5f, -0.5f},	{0.0f, 0.0f}},
		{{0.5f, 0.5f, 0.5f},	{0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.5f},	{1.0f, 1.0f}},

		{{-0.5f, -0.5f, -0.5f},	{1.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f},	{0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.5f},	{0.0f, 1.0f}},
		{{-0.5f, -0.5f, 0.5f},	{1.0f, 1.0f}}
	};

	std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};

	mesh = new Graphics::Mesh(*graphics, verts, indices);

	int width, height, channels;
	stbi_uc* pixels = stbi_load("data/textures/bricks.jpg", &width, &height, &channels, STBI_rgb_alpha);

	if (!pixels)
		throw std::runtime_error("Failed to load default texture!");

	texture = new Graphics::Texture(*graphics, width, height, pixels);

	object = new Graphics::Object(*mesh, *texture);
	object->setScale({ 1.0f, 0.5f, 1.0f });
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