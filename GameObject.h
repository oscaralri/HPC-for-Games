#pragma once
#include <glm/ext/vector_float3.hpp>
#include "Model.h"
#include "LODSystem.h"

// se esta inicializando con un valor por ahora, tendra que cambiarse
struct AABB 
{
	glm::vec3 min = {3.f, -4.f, -7.f};
	glm::vec3 max = {7.f, 0.f, -3.f};
};

class GameObject
{
private:
	glm::vec3 position;
	Model& model;
	Shader& shader;

public:
	GameObject(glm::vec3 p, Model& m, Shader& s) : position(p), model(m), shader(s) {};
	
	glm::vec3 getPosition() { return position; }
	void Render();

	// esto son cosas del frustum culling que no deberian ir aqui pero por ahora
	void cull_AABBs_against_frustum(const Camera& camera, const Array<mat4>& transforms, const Array<AABB>& aabb_list, Array<u32>& out_visible_list)
	bool testAABBinFrustum(glm::mat4& MVP, const AABB& aabb);
};
 