#pragma once

#include "Model.h"

enum class RenderType
{
	Normal,
	Instanced
};

struct Renderable
{
	//std::shared_ptr<Model> model;
	ResourceHandle model;
	std::shared_ptr<Shader> shader;
	RenderType renderType;
};