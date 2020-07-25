#include "HelperDXGraphics.h"

// DX RESOURCE
void DXResource::InitializeAsDefault(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_RESOURCE_DESC* description, D3D12_RESOURCE_STATES setState, D3D12_CLEAR_VALUE* clearValue)
{
	this->currentState = setState;

	HRESULT HR;

	HR = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		description,
		this->currentState,
		clearValue,
		IID_PPV_ARGS(this->resource.GetAddressOf()));
	CheckError(HR, "Failed to initialize this resource.");
}

void DXResource::InitializeAsUpload(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_RESOURCE_DESC* description, D3D12_RESOURCE_STATES setState, D3D12_CLEAR_VALUE* clearValue)
{
	this->currentState = setState;

	HRESULT HR;

	HR = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		description,
		this->currentState,
		clearValue,
		IID_PPV_ARGS(this->resource.GetAddressOf()));
	CheckError(HR, "Failed to initialize this resource.");
}

void DXResource::InitializeFromResource(Microsoft::WRL::ComPtr<ID3D12Resource> newResource, D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_PRESENT)
{
	this->resource = newResource;
	this->currentState = initState;
}

void DXResource::TransitionTo(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, D3D12_RESOURCE_STATES newState)
{
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(this->resource.Get(), this->currentState, newState));
	this->currentState = newState;
}

void DXResource::TransitionTo(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState)
{
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(this->resource.Get(), oldState, newState));
	this->currentState = newState;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DXResource::GetResource()
{
	return this->resource;
}

ID3D12Resource* DXResource::Get()
{
	return this->resource.Get();
}

ID3D12Resource** DXResource::GetAddressOf()
{
	return this->resource.GetAddressOf();
}

D3D12_RESOURCE_STATES DXResource::GetState()
{
	return this->currentState;
}

D3D12_GPU_VIRTUAL_ADDRESS DXResource::GetGPUVirtualAddress()
{
	return this->resource->GetGPUVirtualAddress();
}

void DXResource::Map(unsigned int Subresource, D3D12_RANGE* pReadRange, void** ppData)
{
	HRESULT HR;

	HR = this->resource->Map(Subresource, pReadRange, ppData);
	CheckError(HR, "Failed to map the resource.");
}

void DXResource::Unmap(unsigned int Subresource, D3D12_RANGE* pWrittenRange)
{
	this->resource->Unmap(Subresource, pWrittenRange);
}

void DXResource::SetName(LPCWSTR name)
{
	this->resource->SetName(name);
}

void DXResource::Release()
{
	this->resource->Release();
}

// DX DESCRIPTOR HEAP
void DXDescriptorHeap::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numberOfHeaps, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
	HRESULT HR;

	this->device = device;
	this->maximumHeaps = numberOfHeaps;

	// Describe descriptor heaps;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = this->maximumHeaps;
	heapDesc.Type = type;
	heapDesc.Flags = flags;

	// Create the descriptor heap.
	HR = this->device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(this->mainHeap.GetAddressOf()));
	CheckError(HR, "Failed to create descriptor heap.");

	// Get heap size.
	this->heapSize = this->device->GetDescriptorHandleIncrementSize(type);

	// Get descriptor heap handle.
	this->startCPUHandle = this->mainHeap->GetCPUDescriptorHandleForHeapStart();
	this->startGPUHandle = this->mainHeap->GetGPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE DXDescriptorHeap::GetCPUHandle(unsigned int offsetIndex)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = this->startCPUHandle.ptr + (offsetIndex * heapSize);

	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DXDescriptorHeap::GetGPUHandle(unsigned int offsetIndex)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = this->startGPUHandle.ptr + (offsetIndex * heapSize);

	return handle;
}

ID3D12DescriptorHeap* DXDescriptorHeap::Get()
{
	return this->mainHeap.Get();
}

// MISCELLANEOUS FUNCTIONS
void TransferResourceContent(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, DXResource* fromResource, DXResource* toResource, DXGI_FORMAT format)
{
	// Record the previous state of the resources.
	D3D12_RESOURCE_STATES previousFromState = fromResource->GetState();
	D3D12_RESOURCE_STATES previousToState = toResource->GetState();

	// Transition the resource states for copying the contents.
	fromResource->TransitionTo(commandList, D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
	toResource->TransitionTo(commandList, D3D12_RESOURCE_STATE_RESOLVE_DEST);

	// Transfer the content.
	commandList->ResolveSubresource(toResource->Get(), 0, fromResource->Get(), 0, format);

	// Transition the resources back to its initial state.
	fromResource->TransitionTo(commandList, previousFromState);
	toResource->TransitionTo(commandList, previousToState);
}

unsigned int GetAggregateSize(unsigned int size)
{
	return ((size + 255) & ~255);
}

D3D12_INPUT_ELEMENT_DESC DXVertex::inputEleDesc[3];
unsigned int DXVertex::elements = 3;

D3D12_INPUT_ELEMENT_DESC* DXVertex::GetVertexLayout()
{
	inputEleDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputEleDesc[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputEleDesc[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	return inputEleDesc;
}