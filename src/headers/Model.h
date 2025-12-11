#pragma once

#include <vector>
#include <array>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"
#include "stb_image.h"

// uso struct en lugar de directamnete la lista y ya porque quizas añado más variables/info al struct
struct LODLevel {
	std::vector<Mesh> meshes;
	int distance;
};

class Model
{
public:
	Model(std::string const& path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}
	
	Model(std::vector<std::string>& paths, int increment, bool gamma = false) : gammaCorrection(gamma)
	{
		
		for (size_t i = 0; i < paths.size(); ++i)
		{
			lodIncrement += increment;
			loadModel(paths[i]);
		}
		FindAABBMinMax(getLODs()[0].meshes, minMax);
	}
	
	void Draw(Shader& shader, int lodLevel); 
	void InstancedDraw(Shader& shader, int lodLevel, int amount);

	std::vector<LODLevel> getLODs() { return LODs; }
	std::array<glm::vec3, 2> getMinMax() { return minMax; }

private:
	std::vector<LODLevel> LODs;

	std::vector<Texture> textures_loaded;
	std::string directory;
	bool gammaCorrection;
	int lodIncrement = 0;
	std::array<glm::vec3, 2> minMax;

	void loadModel(std::string const& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
	void FindAABBMinMax(const std::vector<Mesh> meshes, std::array<glm::vec3, 2>& aabb);
};

