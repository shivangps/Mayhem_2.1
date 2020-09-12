#pragma once
#include "HelperDXGraphics.h"
#include "DXFramebuffer.h"

// This header file contains the a class with framebuffer implementation for deferred renderering.

enum DeferedBuffers
{
	FragmentPosition = 0,
	Albedo,
	Normal,
	SpecularGloss
};

class DXDeferedRenderer
{
public:
	DXDeferedRenderer() {}
	~DXDeferedRenderer() {}

private:
	// Descriptor Heaps.
	DXDescriptorHeap rtvHeap = {};
	DXDescriptorHeap depthHeap = {};

	// Creation of Framebuffers.
	DXFramebuffer fragmentPositionFramebuffer = {};
	DXFramebuffer albedoFramebuffer = {};
	DXFramebuffer normalFramebuffer = {};
	DXFramebuffer specularGlossFramebuffer = {};
	DXFramebuffer depthFramebuffer = {};

	// Shader Resource Buffers.
	static const unsigned int numBuffers = 4;
	DXGI_FORMAT gBufferFormat[numBuffers] = { DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT };

	// Function to copy the content of resource from MSAA to non-MSAA resource.
	void CopyResources(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);

public:
	// Function to initialize the RenderFramebuffer.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height, unsigned int multiSamples);
	// Function to create the gBuffers and create and set its views in heaps.
	void CreateGBuffers(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height, unsigned int multiSamples);
	// Function to create the depth buffer and create a view and set it in heaps.
	void CreateDepthBuffers(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height, unsigned int multiSamples);
	// Function to set the RenderFramebuffer.
	void SetFramebuffer(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to remove the RenderFramebuffer.
	void RemoveFramebuffer(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int numRT, D3D12_CPU_DESCRIPTOR_HANDLE* newRTHandle, D3D12_CPU_DESCRIPTOR_HANDLE* newDepthHandle);
	// Function to set fragment position RenderFramebuffer to a handle.
	void SetFragPosFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Function to set color RenderFramebuffer view to a handle.
	void SetColorFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Function to get normal RenderFramebuffer view to a handle.
	void SetNormalFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Function to get specular and gloss RenderFramebuffer view to a handle.
	void SetSpecularFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Function to get the depth buffer view to to a handle.
	void SetDepthFramebufferToHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Function to get number of render targets.
	unsigned int NumberOfRT();
	// Function to get render target format array.
	DXGI_FORMAT* GetRTFormats();
};