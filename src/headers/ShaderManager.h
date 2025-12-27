#pragma once

#include "ResourceStorage.h"
#include "Shader.h"

class ShaderManager 
{
public:
	ResourceHandle LoadShader(const char* vertexPath, const char* fragmentPath)
	{
		Shader shader(vertexPath, fragmentPath);
		ResourceHandle rh = shaderStorage.Create(shader);
		return rh;
	}

	ResourceHandle LoadShaderGeometry(const char* vertexPath, const char* geometryPath, const char* fragmentPath)
	{
		Shader shader(vertexPath, geometryPath, fragmentPath);
		ResourceHandle rh = shaderStorage.Create(shader);
		return rh; 
	}

	Shader* Get(const ResourceHandle& rh)
	{
		return shaderStorage.Get(rh);
	}


	void Destroy(ResourceHandle& rh)
	{
		shaderStorage.Destroy(rh);
	}

private:
	ResourceStorage<Shader> shaderStorage;
};