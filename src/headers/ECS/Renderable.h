#pragma once

#include "Model.h"

enum class RenderType
{
	Normal,
	Instanced,
	Batch
};

struct Renderable
{
	ResourceHandle model;
	ResourceHandle shader;
	RenderType renderType;
	int LodLevel;
};