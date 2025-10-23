#pragma once
#include <glm/ext/vector_float3.hpp>
#include "Model.h"
#include "OptimizeSystem.h"
#include "OptimizeSystem.h"

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
	unsigned int ID;
	Transform transform; 
	Model& model;
	Shader& shader;
	AABB aabb;

	void findAABBMinMax(const std::vector<Mesh> meshes, glm::vec3& aabbMin, glm::vec3& aabbMax);

public:
	 // CAMBIARLO A PRIVADO ESTO ES PARA UN TEST!!!!!!!!!!!!!!!!!!
	GameObject(
		unsigned int id,
		Model& m,
		Shader& sh,
		const glm::vec3& p = glm::vec3(0.f),
		const glm::vec3& r = glm::vec3(0.f),
		const glm::vec3& s = glm::vec3(1.f)
	) :
			ID(id),
			model(m), 
			shader(sh),
			transform(p, r, s) 
	{
		std::vector<LODLevel> lods = model.getLODs();
		findAABBMinMax(lods[0].meshes, aabb.min, aabb.max); // paso los meshes del lod 0 que son los que tienen mas vertices
	};
	
	// getters
	unsigned int getID() { return ID; }
	glm::vec3 getPosition() { return transform.position; }
	glm::vec3 getRotation() { return transform.rotation; }
	glm::vec3 getScale() { return transform.scale; }
	Transform getTransform() { return transform; }
	glm::mat4 getModelMatrix() {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, getPosition());
		model = glm::rotate(model, glm::radians(getRotation().x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(getRotation().y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(getRotation().z), glm::vec3(0, 0, 1));
		model = glm::scale(model, getScale());
		return model;
	}
	AABB getAABB() { return aabb; }
	void Render();
};
 