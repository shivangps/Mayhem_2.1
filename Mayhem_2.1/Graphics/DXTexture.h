#pragma once
#include "HelperDXGraphics.h"

// Header that contains the texture class.
// The texture class that contains a texture and handles all functions related to texture loading and access for shaders.

class DXTexture
{
public:
	DXTexture() {}

private:
	DXResource mainTexture = {};
	DXResource uploadTexture = {};

public:
	// Function to initialize by loading textures from files.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, std::string fileDirectory);
	// Function to create shader resource view from this texture.
	void CreateResourceView(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
};