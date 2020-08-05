#pragma once
#include "HelperDXGraphics.h"

// header that contains the quad mesh class.
// This class simply contains the quad mesh applied for post-processing.

class DXQuadMesh
{
	DXQuadMesh() {}
	~DXQuadMesh() {}

public:
	// It is a singleton.
	static DXQuadMesh* GetInstance()
	{
		static DXQuadMesh* instance = new DXQuadMesh();
		return instance;
	}

private:
	DXResource quadVertexBuffer = {};
	D3D12_VERTEX_BUFFER_VIEW quadVBV = {};

public:
	// Function to initialize the vertices of quad.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device);
	// Function to draw.
	void DrawQuad(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
};