#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Shader.h"

class ShaderStorage
{
private:
	std::unordered_map<std::string, std::shared_ptr<Shader>> shaders; // puntero = program id

	ShaderStorage() = default;
	ShaderStorage(const ShaderStorage&) = delete;
	ShaderStorage& operator=(const ShaderStorage&) = delete;

public:
	static ShaderStorage& Get()
	{
		static ShaderStorage instance;
		return instance;
	}
		
	void Add(const std::string& name, const std::shared_ptr<Shader>& shader)
	{
		shaders[name] = shader;
	}
	std::shared_ptr<Shader> GetShader(const std::string& name) 
	{ 
		auto it = shaders.find(name);
		if (it != shaders.end())
			return it->second;
		return nullptr;
	}
};

