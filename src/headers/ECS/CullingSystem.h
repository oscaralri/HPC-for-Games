#pragma once



#include <memory>
#include <vector>


#include "System.h"
#include "ECSConfig.h"
#include "Camera.h"
#include "Coordinator.h"
#include "AABB.h"
#include "Transform.h"
#include "Renderable.h"

#include "Grid.h"

enum FrustumPlane
{
	LEFT_FRUSTUM, RIGHT_FRUSTUM, BOTTOM_FRUSTUM, TOP_FRUSTUM, NEAR_FRUSTUM, FAR_FRUSTUM
};

struct Plane
{
	glm::vec3 n;
	float d;
};

struct Frustum
{
	Plane planes[6];
};

class CullingSystem : public ECS::System
{
public:
	std::vector<GridCell> FrustumCulling(ECS::Coordinator& coordinator, const std::shared_ptr<Camera>& camera, std::vector<GridCell>& cells);
	Frustum CreateFrustum(glm::mat4 projection, glm::mat4 view);
	Frustum& GetFrustum() { return frustum; }

private:
	Frustum frustum;
};