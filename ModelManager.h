#pragma once

#include "ResourceStorage.h"
#include "Model.h"
#include <EngineResources.h>


class ModelManager
{
public:
	ResourceHandle LoadModel(const std::string& path, bool gamma = false)
	{
		Model model = Model(path);
		ResourceHandle rh = modelStorage.Create(model);
		return rh;
	}

	ResourceHandle LoadModelLOD(const std::vector<std::string>& paths, int increment, bool gamma = false)
	{
		Model model = Model(paths, increment);
		ResourceHandle rh = modelStorage.Create(model);
		return rh;
	}

	Model* Get(const ResourceHandle& rh)
	{
		return modelStorage.Get(rh);
	}

	void Destroy(ResourceHandle& rh)
	{
		modelStorage.Destroy(rh);
	}

private:
	ResourceStorage<Model> modelStorage;
};

