#pragma once


#include "Camera.h"
#include "Object.h"

namespace Graphics {

	/*
	A structure that holds a graphical scene
	*/
	struct Scene {
		Scene(Camera &camera, Object &object);

		Camera & camera;
		Object & object;
		glm::vec3 lightPosition = glm::vec3(1.0f);
		glm::vec3 lightColor = glm::vec3(1.0f);
		glm::vec3 ambientColor = glm::vec3(0.1f);
	};
};