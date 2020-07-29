#pragma once
#include "DXTexture.h"

// Header file that contains the class that has a single instance collection of all loaded textures.

struct DXTextureInfo
{
public:
	std::string fileName;
	DXTexture texture;
};

class DXTextureSystem
{
public:
	DXTextureSystem() {}
	~DXTextureSystem() {}

private:
	std::vector<DXTextureInfo> textures;

public:
	// Function to get the texture index if the textures exists in collection else load a new texture and get the index.
	unsigned int RegisterTexture(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, std::string fileDirectory);
	// Function to create the shader resource view from the texture collection.
	void CreateResourceView(unsigned int index, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
};