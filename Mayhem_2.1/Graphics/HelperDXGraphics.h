#pragma once
#include "GraphicsHeader.h"

// This header file consists of all convinient functions and classes.

// Class to manage the resource.
class DXResource
{
public:
	DXResource()
	{
		this->resource = nullptr;
	}
	~DXResource() {}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	D3D12_RESOURCE_STATES currentState = D3D12_RESOURCE_STATE_PRESENT;

public:
	// Function to initialize resource.
	void InitializeAsDefault(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_RESOURCE_DESC* description, D3D12_RESOURCE_STATES setState, D3D12_CLEAR_VALUE* clearValue);
	void InitializeAsUpload(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_RESOURCE_DESC* description, D3D12_RESOURCE_STATES setState, D3D12_CLEAR_VALUE* clearValue);
	void InitializeFromResource(Microsoft::WRL::ComPtr<ID3D12Resource> newResource, D3D12_RESOURCE_STATES initState);

	// Function to transition the resource to a different state.
	void TransitionTo(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, D3D12_RESOURCE_STATES newState);
	void TransitionTo(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState);
	// Function to get the main resource.
	Microsoft::WRL::ComPtr<ID3D12Resource> GetResource();
	// Function to get the address of the resource.
	ID3D12Resource* Get();
	ID3D12Resource** GetAddressOf();
	// Function to get the state of the resource.
	D3D12_RESOURCE_STATES GetState();
	// Function to GPU virtual address.
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();

	// Function to map the contents of the resource to store resource data.
	void Map(unsigned int Subresource, D3D12_RANGE* pReadRange, void** ppData);
	// Function to unmap the resource.
	void Unmap(unsigned int Subresource, D3D12_RANGE* pWrittenRange);
	// Function to set the name of the resource.
	void SetName(LPCWSTR name);
	// Function to remove the resource from memory.
	void Release();
};

// Class to manage descriptor heap.
class DXDescriptorHeap
{
public:
	DXDescriptorHeap() {}
	~DXDescriptorHeap()
	{
		mainHeap->Release();
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Device5> device = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mainHeap = nullptr;
	unsigned int heapSize = 0;
	unsigned int maximumHeaps = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE startCPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE startGPUHandle;

public:
	// Initialization function.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numberOfHeaps, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
	// Function to get the CPU handle offset from start of heap.
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(unsigned int offsetIndex);
	// Function to get the GPU handle offset from start of heap.
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(unsigned int offsetIndex);
	// Function to get the pointer of the heap.
	ID3D12DescriptorHeap* Get();
};

// Struct to store a vertex. And to get the vertex layout.
struct DXVertex
{
public:
	// Storing vertex data.
	DXVertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT2 texCoord) : position(position), normal(normal), texCoord(texCoord) {}
	DXVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) : position(x, y, z), normal(nx, ny, nz), texCoord(u, v) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texCoord;

public:
	// Vertex layout.
	static D3D12_INPUT_ELEMENT_DESC inputEleDesc[3];
	static unsigned int elements;

	static D3D12_INPUT_ELEMENT_DESC* GetVertexLayout();
};

// Function to transfer the content of the resource from one to another.
void TransferResourceContent(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, DXResource* fromResource, DXResource* toResource, DXGI_FORMAT format);

// Function to give the aggregate memory size for DirectX 12.
unsigned int GetAggregateSize(unsigned int size);

// Function to create constant buffer and get the pointer for updating the values in buffer.
unsigned char* CreateConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device, DXResource* resource, unsigned int size, D3D12_CPU_DESCRIPTOR_HANDLE handle, std::wstring name);