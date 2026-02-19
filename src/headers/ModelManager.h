#pragma once

#include "ResourceStorage.h"
#include "Model.h"

#include <vector>
#include <filesystem>
#include <string>
#include <unordered_map>


class ModelManager
{
public:
	ResourceHandle LoadModel(const std::string& path)
	{
		std::string clean = CleanPath(path);

		auto it = LoadedPaths.find(clean);
		if (it != LoadedPaths.end()) {
			return it->second;
		}

		Model model(path);
		ResourceHandle rh = modelStorage.Create(model);
		LoadedPaths[clean] = rh;

		return rh;
	}

	ResourceHandle LoadModelLOD(const std::vector<std::string>& paths, int increment)
	{

		std::string clean = CleanPath(paths[0]);

		auto it = LoadedPaths.find(clean);
		if (it != LoadedPaths.end()) {
			return it->second;
		}

		Model model(paths, increment);
		ResourceHandle rh = modelStorage.Create(model);
		LoadedPaths[clean] = rh;

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
	std::string CleanPath(const std::string& path) {
		return std::filesystem::path(path).lexically_normal().filename().string();
	}

	std::unordered_map<std::string, ResourceHandle> LoadedPaths;
	ResourceStorage<Model> modelStorage;
};

