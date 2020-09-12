#include "DXMeshSystem.h"

void DXMeshSystem::LoadModel(const Microsoft::WRL::ComPtr<ID3D12Device5> device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, std::string fileLocation, DXModel* model)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(fileLocation, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ConvertToLeftHanded);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		ErrorLog("ASSIMP IMPORT ERROR");
		exit(-1);
	}

	this->ProcessNode(device, commandList, scene->mRootNode, scene, model);

	return;
}

void DXMeshSystem::ProcessNode(const Microsoft::WRL::ComPtr<ID3D12Device5> device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, aiNode* node, const aiScene* scene, DXModel* model)
{
	// Process all mesh nodes if any.
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		model->meshes.push_back(this->ProcessMesh(device, commandList, mesh, scene));
	}
	// Process the all the nodes present as children.
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(device, commandList, node->mChildren[i], scene, model);
	}

	return;
}

DXMesh DXMeshSystem::ProcessMesh(const Microsoft::WRL::ComPtr<ID3D12Device5> device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, aiMesh* mesh, const aiScene* scene)
{
	std::vector<DXVertex> vertices;
	std::vector<unsigned short> indices;

	// For each vertex
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// Load Position.
		DirectX::XMFLOAT3 position = {};
		position.x = mesh->mVertices[i].x;
		position.y = mesh->mVertices[i].y;
		position.z = mesh->mVertices[i].z;

		// Load Normals.
		DirectX::XMFLOAT3 normal = {};
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;

		// Load Texture Coordinates.
		DirectX::XMFLOAT2 texCoord = {};
		if (mesh->mTextureCoords[0])
		{
			texCoord.x = mesh->mTextureCoords[0][i].x;
			texCoord.y = mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(DXVertex(position, normal, texCoord));
	}
	// For each index.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return DXMesh(device, commandList, vertices, indices);
}

unsigned int DXMeshSystem::RegisterModel(const Microsoft::WRL::ComPtr<ID3D12Device5> device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, std::string fileDirectory)
{
	unsigned int modelIndex = 0;

	// Get the file name from directory.
	std::string  fileName = GetFileNameFromDirectory(fileDirectory);

	// Check if the file with same name exists and return the index.
	for (unsigned int i = 0; i < models.size(); i++)
	{
		if (std::strcmp(fileName.c_str(), models[i].fileName.c_str()) == 0)
		{
			return i;
		}
	}

	// If the file name does not exist in the model list then create a new mesh and return its index.
	DXModel model;
	model.fileName = fileName;
	this->LoadModel(device, commandList, fileDirectory, &model);
	modelIndex = this->models.size();
	this->models.push_back(model);

	return modelIndex;
}

void DXMeshSystem::DrawMesh(unsigned int index, unsigned int instances, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	unsigned int meshSize = models[index].meshes.size();
	for (unsigned int i = 0; i < meshSize; i++)
		models[index].meshes[i].DrawWithoutTexture(instances, commandList);
}
