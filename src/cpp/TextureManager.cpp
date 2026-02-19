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

void TextureManager::LoadTextures(const std::string& jsonPath)
{
	auto texConfig = LoadTextureConfig(jsonPath);
	int layers = texConfig.paths.size();

	glGenTextures(1, &globalArrayID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, globalArrayID);

	// reservar memoria para todas las capas
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, texConfig.width, texConfig.height, layers);

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
	
	// puedo poner algun filtro mas (como hacia antes) lo he dejado mas sencillo por la duda
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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


/*
ResourceHandle TextureManager::LoadTexture(const char* path, const std::string& directory, std::string typeName)
{
	std::string newPath = CleanPath(path);
	auto it = LoadedPaths.find(newPath);
	if (it != LoadedPaths.end())
	{
		return it->second;
	}

	Texture tex = LoadTextureFromFile(newPath, directory, typeName);
	ResourceHandle rh = textureStorage.Create(tex);
	LoadedPaths[newPath] = rh;
	return rh;
}
*/

/*
Texture LoadTextureFromFile(const std::string& path, const std::string& directory, std::string typeName)
{
	Texture tex;
	tex.path = path;

	tex.type = typeName;

	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	glGenTextures(1, &tex.id);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, tex.id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return tex;
}



void TextureManager::Destroy(ResourceHandle& handle)
{
	Texture* tex = textureStorage.Get(handle);
	if (!tex) return;

	auto it = LoadedPaths.find(tex->path);
	if (it != LoadedPaths.end())
	{
		LoadedPaths.erase(it);
	}

	glDeleteTextures(1, &tex->id);

	textureStorage.Destroy(handle);
}
*/