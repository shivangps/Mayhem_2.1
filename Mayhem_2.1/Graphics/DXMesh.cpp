#include "DXMesh.h"

DXMesh::DXMesh(const Microsoft::WRL::ComPtr<ID3D12Device5> device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList,
	std::vector<DXVertex> vertices, std::vector<unsigned short> indices)
{
	this->vertices = vertices;
	this->indices = indices;
	this->SetupMesh(device, commandList);
}

void DXMesh::DrawWithoutTexture(unsigned int instances, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &this->vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
	commandList->DrawIndexedInstanced(this->numOfIndices, instances, 0, 0, 0);
}

void DXMesh::SetupMesh(const Microsoft::WRL::ComPtr<ID3D12Device5> device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	// VERTEX INITIALIZATION.
	this->numOfVertices = vertices.size();

	unsigned int sizeOfVertices = sizeof(DXVertex) * this->numOfVertices;

	// Create the vertex buffer to store the vertex in GPU memory.
	this->vertexBuffer.InitializeAsUpload(device, &CD3DX12_RESOURCE_DESC::Buffer(sizeOfVertices), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->vertexBuffer.SetName(L"Mesh Vertex Buffer");

	// Store the vertex in the buffer.
	unsigned char* pMeshVertexDataBegin = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	this->vertexBuffer.Map(0, &readRange, reinterpret_cast<void**>(&pMeshVertexDataBegin));
	memcpy(pMeshVertexDataBegin, &this->vertices[0], sizeOfVertices);
	this->vertexBuffer.Unmap(0, nullptr);

	// Define the vertex buffer in a view;
	this->vertexBufferView.BufferLocation = this->vertexBuffer.GetGPUVirtualAddress();
	this->vertexBufferView.StrideInBytes = sizeof(DXVertex);
	this->vertexBufferView.SizeInBytes = sizeOfVertices;

	// INDEX INITIALIZATION.
	this->numOfIndices = indices.size();

	unsigned int sizeOfIndices = sizeof(unsigned short) * this->numOfIndices;

	// Create a default buffer for index.
	this->indexDefaultBuffer.InitializeAsDefault(device, &CD3DX12_RESOURCE_DESC::Buffer(sizeOfIndices), D3D12_RESOURCE_STATE_COPY_DEST, nullptr);
	this->indexDefaultBuffer.SetName(L"Mesh Index Default Buffer");

	// Create an upload buffer for index upload to GPU memory.
	this->indexUploadBuffer.InitializeAsUpload(device, &CD3DX12_RESOURCE_DESC::Buffer(sizeOfVertices), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->indexUploadBuffer.SetName(L"Mesh Index Upload Buffer");

	D3D12_SUBRESOURCE_DATA indexData = {};
	indexData.pData = reinterpret_cast<BYTE*>(&indices[0]);
	indexData.RowPitch = sizeOfIndices;
	indexData.SlicePitch = sizeOfIndices;

	// Copy the contents of subresource to another buffer.
	UpdateSubresources(commandList.Get(), this->indexDefaultBuffer.Get(), this->indexUploadBuffer.Get(), 0, 0, 1, &indexData);

	this->indexDefaultBuffer.TransitionTo(commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	// Define the index buffer in a view.
	this->indexBufferView.BufferLocation = this->indexDefaultBuffer.GetGPUVirtualAddress();
	this->indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	this->indexBufferView.SizeInBytes = sizeOfIndices;
}
