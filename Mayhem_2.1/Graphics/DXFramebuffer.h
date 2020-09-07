#pragma once
#include "GraphicsHeader.h"
#include "HelperDXGraphics.h"

// Header file contains the implementation of a framebuffer.

class DXFramebuffer
{
private:
	DXResource renderBuffer = {};
	DXResource textureBuffer = {};
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

	bool isDepthbuffer = false;
	DXGI_FORMAT renderFormat = {};
	DXGI_FORMAT depthRtFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DXGI_FORMAT depthSrFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

public:
	DXFramebuffer() {}

	// Function to initialize the framebuffer as a render buffer and not depth buffer.
	void InitializeAsRenderbuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device,
		DXGI_FORMAT format, unsigned int width, unsigned int height, unsigned int multiSamples, std::wstring framebufferName);
	// Function to initialize the framebuffer as a depth buffer and not a render buffer.
	void InitializeAsDepthbuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device,
		 unsigned int width, unsigned int height, unsigned int multiSamples, std::wstring depthbufferName);
	// Set the framebuffer render target to a cpu descriptor handle.
	void SetFramebufferToRTVHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Set the framebuffer depth buffer to a cpu descriptor handle.
	void SetFramebufferToDSVHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Set the framebuffer as a shader resource to a cpu descriptor handle.
	void SetFramebufferToSRVHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Function to copy the image from multi sampled render target resource to unsampled shader resource.
	void CopyResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);

	// Get function to tell if this is initialized as depth buffer.
	bool IsDepthFramebuffer() { return this->isDepthbuffer; }
};