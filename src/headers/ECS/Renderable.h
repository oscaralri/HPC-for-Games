#pragma once

#include "Model.h"

enum class RenderType
{
	Simple,
	Instanced
};

struct Renderable
{
	ResourceHandle model;
	ResourceHandle shader;
	RenderType renderType;
	int LodLevel;
};