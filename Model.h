#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"
#include "stb_image.h"

/*
struct LODLevel {
	std::vector<Mesh> meshes;
};
*/
class Model
{
private:
	//std::vector<LODLevel> lods; // para cada lod tiene unos meshes
	
	std::vector<Texture> textures_loaded; 
	std::string directory;
	bool gammaCorrection;
	
	void loadModel(std::string const& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

public:
	// ESTAS TRES PASARLAS A PRIVATE!!!!!!!!!!!!!!!!!!!!!!
	int loadingLod = 0;
	int lodLevels = 2;
	int activeLod = 0;
	std::vector<Mesh> meshes; // mesh activo 
	std::vector<Mesh> meshes1; // lod0
	
	Model() {};
	Model(std::string const& path, std::string const& path1, bool gamma = false) : gammaCorrection(gamma)
	{
		// for cada lodlevel hacer un load model pasandole el path de su malla y hacinedo que se guardase en el meshes 
			// correspondinte
			
		for (int i = 0; i < 2; i++)
		{
			loadingLod = i;
			if(i == 0) loadModel(path); // el prime
			if (i == 1) loadModel(path1);
		}
	}
	void Draw(Shader& shader);
};

