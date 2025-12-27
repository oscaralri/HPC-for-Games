#pragma once

#include "Model.h"

enum class RenderType
{
	Normal,
	Instanced
};

struct Renderable
{
	ResourceHandle model;
	ResourceHandle shader;
	RenderType renderType;
};