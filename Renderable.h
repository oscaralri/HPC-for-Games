#pragma once

#include "Model.h"

struct Renderable
{
	std::shared_ptr<Model> model;
	std::shared_ptr<Shader> shader;
};