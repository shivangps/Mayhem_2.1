#include "RenderFramebuffer.h"

void RenderFramebuffer::SetFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int index, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Format = this->gBufferFormat[index];
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	device->CreateShaderResourceView(this->gBuffersSRV[index].Get(), &srvDesc, handle);
}

void RenderFramebuffer::CopyResources(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	for (unsigned int i = 0; i < this->numBuffers; i++)
	{
		TransferResourceContent(commandList, &this->gBuffersRTV[i], &this->gBuffersSRV[i], this->gBufferFormat[i]);
	}
	TransferResourceContent(commandList, &this->depthBufferRTV, &this->depthBufferSRV, this->dsvSRFormat);
}

void RenderFramebuffer::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height, unsigned int multiSamples)
{
	// Initialize the heaps.
	this->rtvHeap.Initialize(device, this->numBuffers, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	this->depthHeap.Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	// Initialize the resources.

	//-------------------------
	// Shader resource buffers.
	this->CreateGBuffers(device, width, height, multiSamples);

	//----------------------------------
	// Initializing depth stencil buffer.
	this->CreateDepthBuffers(device, width, height, multiSamples);

}

void RenderFramebuffer::CreateGBuffers(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height, unsigned int multiSamples)
{
	// Description of the render target resource.
	D3D12_RESOURCE_DESC resourceDesc = {};
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = multiSamples;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	// Describe clear value.
	D3D12_CLEAR_VALUE clearVal;
	clearVal.Color[0] = 0.0f;
	clearVal.Color[1] = 0.0f;
	clearVal.Color[2] = 0.0f;
	clearVal.Color[3] = 1.0f;

	// Creation of render target resource.
	for (unsigned int i = 0; i < this->numBuffers; i++)
	{
		resourceDesc.Format = this->gBufferFormat[i];
		clearVal.Format = this->gBufferFormat[i];
		this->gBuffersRTV[i].InitializeAsDefault(device, &resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearVal);
		this->gBuffersRTV[i].SetName(L"G Buffer RTV");
	}

	// Description of shader resource.
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	for (unsigned int i = 0; i < this->numBuffers; i++)
	{
		resourceDesc.Format = this->gBufferFormat[i];
		this->gBuffersSRV[i].InitializeAsDefault(device, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	}

	// Set as render target view heaps.
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
	for (unsigned int i = 0; i < this->numBuffers; i++)
	{
		rtvDesc.Format = this->gBufferFormat[i];
		device->CreateRenderTargetView(this->gBuffersRTV[i].Get(), &rtvDesc, this->rtvHeap.GetCPUHandle(i));
	}
}

void RenderFramebuffer::CreateDepthBuffers(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height, unsigned int multiSamples)
{
	D3D12_RESOURCE_DESC depthResourceDesc;
	ZeroMemory(&depthResourceDesc, sizeof(depthResourceDesc));
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Alignment = 0;
	depthResourceDesc.SampleDesc.Count = multiSamples;
	depthResourceDesc.SampleDesc.Quality = 0;
	depthResourceDesc.MipLevels = 1;
	depthResourceDesc.Format = this->dsvRTFormat;
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Width = width;
	depthResourceDesc.Height = height;
	depthResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearVal = { this->dsvRTFormat, 1.0f };

	this->depthBufferRTV.InitializeAsDefault(device, &depthResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearVal);

	depthResourceDesc.SampleDesc.Count = 1;
	this->depthBufferSRV.InitializeAsDefault(device, &depthResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, &clearVal);

	// Set as depth resource view in heap.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Format = this->dsvRTFormat;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(this->depthBufferRTV.Get(), &dsvDesc, this->depthHeap.GetCPUHandle(0));
}

void RenderFramebuffer::SetFramebuffer(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	const float clearColorBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	for (unsigned int i = 0; i < this->numBuffers; i++)
	{
		commandList->ClearRenderTargetView(this->rtvHeap.GetCPUHandle(i), clearColorBlack, 0, nullptr);
	}
	commandList->ClearDepthStencilView(this->depthHeap.GetCPUHandle(0), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0xff, 0, nullptr);

	commandList->OMSetRenderTargets(this->numBuffers, &rtvHeap.GetCPUHandle(0), true, &this->depthHeap.GetCPUHandle(0));
}

void RenderFramebuffer::RemoveFramebuffer(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int numRT, D3D12_CPU_DESCRIPTOR_HANDLE* newRTHandle, D3D12_CPU_DESCRIPTOR_HANDLE* newDepthHandle)
{
	commandList->OMSetRenderTargets(numRT, newRTHandle, true, newDepthHandle);
	this->CopyResources(commandList);
}

void RenderFramebuffer::SetFragPosFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	this->SetFramebufferToHandle(device, 0, handle);
}

void RenderFramebuffer::SetColorFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	this->SetFramebufferToHandle(device, 1, handle);
}

void RenderFramebuffer::SetNormalFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	this->SetFramebufferToHandle(device, 2, handle);
}

void RenderFramebuffer::SetSpecularFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	this->SetFramebufferToHandle(device, 3, handle);
}

void RenderFramebuffer::SetDepthFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Format = this->dsvSRFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	device->CreateShaderResourceView(this->depthBufferSRV.Get(), &srvDesc, handle);
}

unsigned int RenderFramebuffer::NumberOfRT()
{
	return this->numBuffers;
}

DXGI_FORMAT* RenderFramebuffer::GetRTFormats()
{
	return this->gBufferFormat;
}
