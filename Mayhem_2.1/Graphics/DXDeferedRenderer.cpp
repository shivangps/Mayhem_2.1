#include "DXDeferedRenderer.h"

void DXDeferedRenderer::CopyResources(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	this->fragmentPositionFramebuffer.CopyResource(commandList);
	this->albedoFramebuffer.CopyResource(commandList);
	this->normalFramebuffer.CopyResource(commandList);
	this->specularGlossFramebuffer.CopyResource(commandList);
	this->depthFramebuffer.CopyResource(commandList);
}

void DXDeferedRenderer::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height, unsigned int multiSamples)
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

void DXDeferedRenderer::CreateGBuffers(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height, unsigned int multiSamples)
{
	this->fragmentPositionFramebuffer.InitializeAsRenderbuffer(device, this->gBufferFormat[DeferedBuffers::FragmentPosition], width, height, multiSamples, L"Fragment Position");
	this->albedoFramebuffer.InitializeAsRenderbuffer(device, this->gBufferFormat[DeferedBuffers::Albedo], width, height, multiSamples, L"Albedo");
	this->normalFramebuffer.InitializeAsRenderbuffer(device, this->gBufferFormat[DeferedBuffers::Normal], width, height, multiSamples, L"Normal");
	this->specularGlossFramebuffer.InitializeAsRenderbuffer(device, this->gBufferFormat[DeferedBuffers::SpecularGloss], width, height, multiSamples, L"Specular Gloss");

	this->fragmentPositionFramebuffer.SetFramebufferToRTVHandle(device, this->rtvHeap.GetCPUHandle(DeferedBuffers::FragmentPosition));
	this->albedoFramebuffer.SetFramebufferToRTVHandle(device, this->rtvHeap.GetCPUHandle(DeferedBuffers::Albedo));
	this->normalFramebuffer.SetFramebufferToRTVHandle(device, this->rtvHeap.GetCPUHandle(DeferedBuffers::Normal));
	this->specularGlossFramebuffer.SetFramebufferToRTVHandle(device, this->rtvHeap.GetCPUHandle(DeferedBuffers::SpecularGloss));
}

void DXDeferedRenderer::CreateDepthBuffers(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height, unsigned int multiSamples)
{
	this->depthFramebuffer.InitializeAsDepthbuffer(device, width, height, multiSamples, L"Depth");
	this->depthFramebuffer.SetFramebufferToDSVHandle(device, this->depthHeap.GetCPUHandle(0));
}

void DXDeferedRenderer::SetFramebuffer(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	const float clearColorBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	for (unsigned int i = 0; i < this->numBuffers; i++)
	{
		commandList->ClearRenderTargetView(this->rtvHeap.GetCPUHandle(i), clearColorBlack, 0, nullptr);
	}
	commandList->ClearDepthStencilView(this->depthHeap.GetCPUHandle(0), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0xff, 0, nullptr);

	commandList->OMSetRenderTargets(this->numBuffers, &rtvHeap.GetCPUHandle(0), true, &this->depthHeap.GetCPUHandle(0));
}

void DXDeferedRenderer::RemoveFramebuffer(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int numRT, D3D12_CPU_DESCRIPTOR_HANDLE* newRTHandle, D3D12_CPU_DESCRIPTOR_HANDLE* newDepthHandle)
{
	commandList->OMSetRenderTargets(numRT, newRTHandle, true, newDepthHandle);
	this->CopyResources(commandList);
}

void DXDeferedRenderer::SetFragPosFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	this->fragmentPositionFramebuffer.SetFramebufferToSRVHandle(device, handle);
}

void DXDeferedRenderer::SetColorFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	this->albedoFramebuffer.SetFramebufferToSRVHandle(device, handle);
}

void DXDeferedRenderer::SetNormalFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	this->normalFramebuffer.SetFramebufferToSRVHandle(device, handle);
}

void DXDeferedRenderer::SetSpecularFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	this->specularGlossFramebuffer.SetFramebufferToSRVHandle(device, handle);
}

void DXDeferedRenderer::SetDepthFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	this->depthFramebuffer.SetFramebufferToSRVHandle(device, handle);
}

unsigned int DXDeferedRenderer::NumberOfRT()
{
	return this->numBuffers;
}

DXGI_FORMAT* DXDeferedRenderer::GetRTFormats()
{
	return this->gBufferFormat;
}
