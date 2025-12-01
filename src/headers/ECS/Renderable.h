#pragma once

#include "Model.h"

enum class RenderType
{
	Normal,
	RenderInstanced
};

struct Renderable
{
	std::shared_ptr<Model> model;
	std::shared_ptr<Shader> shader;
	RenderType renderType;
};