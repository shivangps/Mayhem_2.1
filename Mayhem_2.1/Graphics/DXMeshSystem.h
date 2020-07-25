#pragma once
#include "DXMesh.h"

// This header contains the class DXMeshSystem.
// Following the ECS this system contins a collection of single instance mesh.
// And if instructed draws one of the meshes.

struct DXModel
{
public:
	std::string fileName;
	std::vector<DXMesh> meshes;
};

class DXMeshSystem
{
public:
	DXMeshSystem() {}
	~DXMeshSystem() {}

private:
	std::vector<DXModel> models;

	// Function to load meshes from a 3D file.
	void LoadModel(const Microsoft::WRL::ComPtr<ID3D12Device5> device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, 
		std::string fileLocation, DXModel* model);
	// Function to process each node mesh.
	void ProcessNode(const Microsoft::WRL::ComPtr<ID3D12Device5> device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList,
		aiNode* node, const aiScene* scene, DXModel* model);
	// Function to load a mesh.
	DXMesh ProcessMesh(const Microsoft::WRL::ComPtr<ID3D12Device5> device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, 
		aiMesh* mesh, const aiScene* scene);
public:
	// Function to get the model in collection of models and load a model if it does not exist in collection.
	unsigned int RegisterModel(const Microsoft::WRL::ComPtr<ID3D12Device5> device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, std::string fileDirectory);
	// Function to draw the mesh from the collection.
	void DrawMesh(unsigned int index, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
};