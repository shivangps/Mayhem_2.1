#include "DXQuadMesh.h"

void DXQuadMesh::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	DXQuadVertex quad[] =
	{
		DXQuadVertex(-1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		DXQuadVertex(-1.0f,-1.0f, 0.0f, 0.0f, 0.0f),
		DXQuadVertex( 1.0f, 1.0f, 0.0f, 1.0f, 1.0f),
		DXQuadVertex( 1.0f,-1.0f, 0.0f, 1.0f, 0.0f)
	};
	unsigned int quadBufferSize = sizeof(quad);

	// Create vertex buffer resource for the quad.
	this->quadVertexBuffer.InitializeAsUpload(device, &CD3DX12_RESOURCE_DESC::Buffer(quadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->quadVertexBuffer.SetName(L"Quad Vertex Buffer");

	// Fill the quad vertex data.
	unsigned char* pQuadVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);
	this->quadVertexBuffer.Map(0, &readRange, reinterpret_cast<void**>(&pQuadVertexDataBegin));
	memcpy(pQuadVertexDataBegin, quad, quadBufferSize);
	this->quadVertexBuffer.Unmap(0, nullptr);

	// Initialize vertex buffer view.
	this->quadVBV.BufferLocation = this->quadVertexBuffer.GetGPUVirtualAddress();
	this->quadVBV.SizeInBytes = quadBufferSize;
	this->quadVBV.StrideInBytes = sizeof(DXQuadVertex);
}

void DXQuadMesh::DrawQuad(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	commandList->IASetVertexBuffers(0, 1, &this->quadVBV);
	commandList->DrawInstanced(4, 1, 0, 0);
}