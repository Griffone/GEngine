#include "Object.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace Graphics;

Object::Object(Mesh &mesh, Texture &diffuseTexture, Texture &normalMap) : mesh(mesh), diffuseTexture(diffuseTexture), normalMap(normalMap) {}

void Object::setScale(const glm::vec3 &s) {
	scale = s;
	transformationMatrixIsCorrect = false;
}

void Object::setPosition(const glm::vec3 &p) {
	position = p;
	transformationMatrixIsCorrect = false;
}

void Object::setRotation(const glm::vec3 &r) {
	rotation = r;
	transformationMatrixIsCorrect = false;
}

glm::mat4 Object::getTransformationMatrix() {
	if (!transformationMatrixIsCorrect) {
		transformationMatrix = glm::mat4(1.0f);
		transformationMatrix = glm::scale(transformationMatrix, scale);
		transformationMatrix = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z) * transformationMatrix;
		transformationMatrix = glm::translate(transformationMatrix, position);
		transformationMatrixIsCorrect = true;
	}
	return transformationMatrix;
}
