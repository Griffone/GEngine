#pragma once

#include "Mesh.h"
#include "Texture.h"

namespace Graphics {
	/*
		A renderable object
	*/
	class Object {
		friend Context;
	public:
		Object(Mesh &mesh, Texture &texture);
		~Object() = default;

		/// Set object's scaling factor
		void setScale(const glm::vec3 &);
		/// Set object's position in cartesian coordinates
		void setPosition(const glm::vec3 &);
		/// Set object's euler rotation angles
		void setRotation(const glm::vec3 &);

		glm::mat4 getTransformationMatrix();
	private:
		Texture & texture;
		Mesh & mesh;

		bool transformationMatrixIsCorrect = false;;
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
		glm::mat4 transformationMatrix;
	};
}