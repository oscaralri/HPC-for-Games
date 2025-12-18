#pragma once
#include <ResourceStorage.h>
#include <unordered_map>
#include <string>

#include <string>
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

struct Texture
{
	unsigned int id;
	std::string type; // difusse or specular
	std::string path;
};

class TextureManager
{
public:
	ResourceHandle LoadTexture(const std::string& path)
	{
		auto it = LoadedPaths.find(path);
		if (it != LoadedPaths.end())
		{
			return it->second;
		}

		Texture tex = LoadTextureFromFile(path);
		ResourceHandle rh = textureStorage.Create(tex);
		LoadedPaths[path] = rh;
		return rh;
	}

	Texture* Get(const ResourceHandle& rh)
	{
		return textureStorage.Get(rh);
	}

	void Destroy(ResourceHandle& handle)
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

private:
	Texture LoadTextureFromFile(const std::string& path)
	{
		Texture tex;
		tex.path = path;
		std::string directory = path.substr(0, path.find_last_of('/'));

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

	ResourceStorage<Texture> textureStorage;
	std::unordered_map<std::string, ResourceHandle> LoadedPaths;

};

