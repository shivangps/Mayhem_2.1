#include "DXTextureSystem.h"

unsigned int DXTextureSystem::RegisterTexture(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, std::string fileDirectory)
{
	unsigned int textureIndex = 0;

	// Get the file name from directory.
	std::string fileName = GetFileNameFromDirectory(fileDirectory);

	// Check if the file with same name exists and return the index.
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		if (std::strcmp(fileName.c_str(), textures[i].fileName.c_str()) == 0)
		{
			return i;
		}
	}

	// If the file name does not exist in the texture list then create a new texture and return its index.
	DXTextureInfo newTextureInfo;
	newTextureInfo.texture.Initialize(device, commandList, fileDirectory);
	newTextureInfo.fileName = fileName;
	textureIndex = textures.size();
	textures.push_back(newTextureInfo);

	return textureIndex;
}

void DXTextureSystem::CreateResourceView(unsigned int index, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	this->textures[index].texture.CreateResourceView(device, handle);
}
