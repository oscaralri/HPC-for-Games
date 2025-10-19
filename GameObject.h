#pragma once
#include <glm/ext/vector_float3.hpp>
#include "Model.h"
#include "LODSystem.h"

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation; // hacer la rotacion asi implica euler (rotacion para cada eje)
	glm::vec3 scale;

	Transform(
		const glm::vec3& p = glm::vec3(0.f),
		const glm::vec3& r = glm::vec3(0.f),
		const glm::vec3& s = glm::vec3(1.f))
		: position(p), rotation(r), scale(s)
	{}
};

class GameObject
{
private:
	Transform transform; 
	Model& model;
	Shader& shader;

public:
	GameObject(
		Model& m,
		Shader& sh,
		const glm::vec3& p = glm::vec3(0.f),
		const glm::vec3& r = glm::vec3(0.f),
		const glm::vec3& s = glm::vec3(1.f)
		) : 
			model(m), 
			shader(sh),
			transform(p, r, s) {};
	
	glm::vec3 getPosition() { return transform.position; }
	glm::vec3 getRotation() { return transform.rotation; }
	glm::vec3 getScale() { return transform.scale; }
	Transform getTransform() { return transform; }
	
	void Render(Shader& s);
};
 