#include "Model.h"

void Model::FindAABBMinMax(const std::vector<Mesh> meshes, std::array<glm::vec3, 2>& aabb)
{
	glm::vec3 maxValues = glm::vec3(std::numeric_limits<float>::lowest());
	glm::vec3 minValues = glm::vec3(std::numeric_limits<float>::max());

	for (const auto& mesh : meshes)
	{
		for (const auto& vertex : mesh.vertices)
			{
			minValues.x = std::min(minValues.x, vertex.Position.x);
			minValues.y = std::min(minValues.y, vertex.Position.y);
			minValues.z = std::min(minValues.z, vertex.Position.z);

			maxValues.x = std::max(maxValues.x, vertex.Position.x);
			maxValues.y = std::max(maxValues.y, vertex.Position.y);
			maxValues.z = std::max(maxValues.z, vertex.Position.z);
		}
	}

	aabb[0] = minValues;
	aabb[1] = maxValues;
}

void Model::Draw(Shader& shader, int lodLevel)
{
	std::vector<Mesh> meshes = LODs[lodLevel].meshes;
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

void Model::InstancedDraw(Shader& shader, int lodLevel, int amount)
{
	std::vector<Mesh> meshes = LODs[lodLevel].meshes;
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].InstancedDraw(shader, amount);
	}
}

void Model::loadModel(std::string const& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace); // convertir caras a triangulos || invertir cordsUV

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);

	std::cout << "lods cuando " << path << ": " << LODs.size() << std::endl;
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	std::vector<Mesh> meshesToLoad;

	// all the node meshes 
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshesToLoad.push_back(processMesh(mesh, scene));

		LODLevel lodLevel;
		lodLevel.distance = lodIncrement;
		lodLevel.meshes = meshesToLoad;
		LODs.push_back(lodLevel);
	}

	// same for each children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<ResourceHandle> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		// tex coords
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;

			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;

			// bitanget
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else
			vertex.TexCoords = glm::vec2(0.f, 0.f);

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	// Textures
	std::vector<ResourceHandle> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	std::vector<ResourceHandle> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	std::vector<ResourceHandle> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

	std::vector<ResourceHandle> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	return Mesh(vertices, indices, textures);
}

std::vector<ResourceHandle> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<ResourceHandle> textureHandles;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		ResourceHandle textureHandle;
		textureHandle = EngineResources::GetTextureManager().LoadTexture(str.C_Str(), typeName);
		textureHandles.push_back(textureHandle);

		// la idea es que esto que se ha hecho tiene que conseguir lo mismo
			// definir una textura con su id, type y path
			// luego se guarda en el textureStorage
			// en el model se guarda la referencia con un handle
	}

	return textureHandles;
};
