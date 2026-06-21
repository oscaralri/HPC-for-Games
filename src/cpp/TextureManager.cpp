#include "TextureManager.h"

struct TextureConfig {
	int width;
	int height;
	std::vector<std::string> paths;
};

std::string CleanPath(const std::string& path)
{
	return std::filesystem::path(path).lexically_normal().filename().string();
}

std::string CleanPath(const char* path)
{
	return std::filesystem::path(path).lexically_normal().filename().string();
}

TextureConfig LoadTextureConfig(const std::string& jsonPath) {
	std::ifstream file(jsonPath);
	json data = json::parse(file);

	TextureConfig config;
	config.width = data["texture_array"]["width"];
	config.height = data["texture_array"]["height"];

	for (const auto& path : data["texture_array"]["files"]) {
		config.paths.push_back(path);
	}

	return config;
}

void TextureManager::LoadTextures(const std::string& jsonPath, TextureType type)
{
	auto texConfig = LoadTextureConfig(jsonPath);
	int layers = texConfig.paths.size();

	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
	textureArrayIDs[type] = id;

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 12, GL_RGBA8, texConfig.width, texConfig.height, layers);

	for (int i = 0; i < layers; i++)
	{
		int width, height, nrComponents;
		unsigned char* data = stbi_load(texConfig.paths[i].c_str(), &width, &height, &nrComponents, 4);

		if (data)
		{
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
			
			std::string cleanPath = CleanPath(texConfig.paths[i]);
			LoadedPaths[cleanPath] = (float)i;
		}	
	}
	
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

float TextureManager::GetTextureIndex(const char* path)
{
	std::string newPath = CleanPath(path);
	auto it = LoadedPaths.find(newPath);
	if (it != LoadedPaths.end())
	{
		return it->second;
	}
}

Texture* TextureManager::Get(const ResourceHandle& rh)
{
	return textureStorage.Get(rh);
}