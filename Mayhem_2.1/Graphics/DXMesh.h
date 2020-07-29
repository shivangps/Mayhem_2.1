#pragma once
#include "HelperDXGraphics.h"

// This header include a class to load and store the mesh from a file.

class DXMesh
{
public:
	DXMesh(const Microsoft::WRL::ComPtr<ID3D12Device5> device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, 
		std::vector<DXVertex> vertices, std::vector<unsigned short> indices);

	// Function to draw the mesh.
	void DrawWithoutTexture(unsigned int instances, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);

private:
	// Mesh data to be stored in vertices and indices.
	// Vertex Attributes.
	std::vector<DXVertex> vertices = {};
	DXResource vertexBuffer = {};
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	unsigned int numOfVertices = 0;

	// Index Attributes.
	std::vector<unsigned short> indices = {};
	DXResource indexDefaultBuffer = {};
	DXResource indexUploadBuffer = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
	unsigned int numOfIndices = {};

	// Function to setup the mesh and create the vertex buffer.
	void SetupMesh(const Microsoft::WRL::ComPtr<ID3D12Device5> device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
};