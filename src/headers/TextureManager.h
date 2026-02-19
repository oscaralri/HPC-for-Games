#pragma once

#include <unordered_map>
#include <string>
#include <filesystem>

#include <string>
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>
#include <json.hpp>
#include <fstream>

#include "ResourceStorage.h"

using json = nlohmann::json;
/*
struct TextureHandle
{
	// esto sera mas necesario (aunque quizas no aqui) cuando/si trabajo con un array de arrays de texturas
	float TexIndex;
};
*/

struct Texture
{
	unsigned int id;
	std::string type; // difusse or specular
	std::string path;
};

class TextureManager
{
public:
	void LoadTextures(const std::string& jsonPath);
	float GetTextureIndex(const char* path);
	GLuint GetGlobalArrayID() { return globalArrayID; }

	//ResourceHandle LoadTexture(const char* path, const std::string& directory, std::string typeName);
	//void GenerateTextureBuffer();
	Texture* Get(const ResourceHandle& rh);
	//void Destroy(ResourceHandle& handle);

private:
	ResourceStorage<Texture> textureStorage;
	std::unordered_map<std::string, float> LoadedPaths;
	GLuint globalArrayID;
};

