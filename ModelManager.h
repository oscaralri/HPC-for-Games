#pragma once

#include "ResourceStorage.h"
#include "Model.h"

class ModelManager
{
public:
	// la cosa es mantener la misma logica de creacion simplemente 
		// cuando acabe de crear las cosas donde se guardan es en el modelStorage
	ResourceHandle LoadModel(const std::string& path, bool gamma = false);
	ResourceHandle LoadModelLOD(const std::vector<std::string>& paths, int lodIncrement, bool gamma = false);

	Model* Get(const ResourceHandle& handle);
	void Destroy(ResourceHandle& handle);

private:
	ResourceStorage<Model> modelStorage;

	std::string directory;
	std::vector<LODLevel> LODs;
	int lodIncrement = 0; // esto se podria cambiar a uint32_t

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);



};

