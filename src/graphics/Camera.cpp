#include "Camera.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Graphics::Camera::Camera(const glm::vec3 & position, const glm::vec3 & target, float fov) : position(position), target(target), fov(fov) {}

void Graphics::Camera::setPosition(const glm::vec3 &p) {
	position = p;
	viewIsCorrect = false;
	pvIsCorrect = false;
}

void Graphics::Camera::setTarget(const glm::vec3 &t) {
	target = t;
	viewIsCorrect = false;
	pvIsCorrect = false;
}

void Graphics::Camera::setFOV(float _fov) {
	fov = _fov;
	pvIsCorrect = false;
}

glm::mat4 Graphics::Camera::getProjectionViewMatrix() {
	if (!pvIsCorrect) {
		if (!viewIsCorrect) {
			viewMatrix = glm::lookAt(position, target, { 0.0f, 1.0f, 0.0f });
			viewIsCorrect = true;
		}
		pvMatrix = glm::perspective(fov, aspectRatio, 0.1f, 10.0f) * viewMatrix;
		pvIsCorrect = true;
	}
	return pvMatrix;
}

glm::mat4 Graphics::Camera::getViewMatrix() {
	if (!viewIsCorrect) {
		viewMatrix = glm::lookAt(position, target, { 0.0f, 1.0f, 0.0f });
		viewIsCorrect = true;
	}
	return viewMatrix;
}

void Graphics::Camera::setAspectRatio(float _aspectRatio) {
	pvIsCorrect = pvIsCorrect && aspectRatio == _aspectRatio;
	aspectRatio = _aspectRatio;
}

