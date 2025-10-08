#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"
#include "stb_image.h"


struct LODLevel {
	std::vector<Mesh> meshes;
};

class Model
{
private:
	std::vector<LODLevel> LODs; 
	
	std::vector<Texture> textures_loaded; 
	std::string directory;
	bool gammaCorrection;
	
	void loadModel(std::string const& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

public:
	Model(std::string const& path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}

	Model(std::vector<std::string> paths, bool gamma = false) : gammaCorrection(gamma)
	{
		for (int i = 0; i < paths.size(); i++)
		{
			loadModel(paths[0]);
		}
		
	}


	void Draw(Shader& shader);
};

