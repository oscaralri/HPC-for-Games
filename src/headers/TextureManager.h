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

enum TextureType
{
	Diffuse,
	Specular
};

using json = nlohmann::json;

struct Texture
{
	unsigned int id;
	std::string type; // difusse or specular
	std::string path;
};

class TextureManager
{
public:
	void LoadTextures(const std::string& jsonPath, TextureType type);
	float GetTextureIndex(const char* path);
	GLuint GetTextureArrayID(TextureType type) 
	{
		auto it = textureArrayIDs.find(type);
		if (it != textureArrayIDs.end()) { return it->second; }
		return 0;
	}
	Texture* Get(const ResourceHandle& rh);

private:
	ResourceStorage<Texture> textureStorage;
	std::unordered_map<std::string, float> LoadedPaths;
	std::unordered_map<TextureType, GLuint> textureArrayIDs;
};

