#pragma once
#include "HelperDXGraphics.h"

// This header constains the base class that loads and defines the DXShader.
// It also constains convinience functions for shader initialization.

class DXRenderEngine;

// Class definition for DXShader.
class DXShader
{
public:
	DXShader() 
	{
		this->CreateDefaultSampler();
	}
	~DXShader() {}

protected:
	Microsoft::WRL::ComPtr<ID3DBlob> vertexDXShader = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelDXShader = nullptr;

	unsigned int numberOfCBV = 0;
	unsigned int numberOfSRV = 0;
	unsigned int numberOfSlots = 0;

	D3D12_STATIC_SAMPLER_DESC sampler = {};
	std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters = {};
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	// Function to create DXShader from a file in directory.
	void CreateDXShaderFromFile(std::string vertexFilePath, std::string pixelFilePath);
	// Function to create the default sampler.
	void CreateDefaultSampler();
	// Function to create the root signature.
	void CreateRootSignature(Microsoft::WRL::ComPtr<ID3D12Device5> device);
	// Function to create descriptor table.
	void CreateDescriptorTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_DESCRIPTOR_RANGE_TYPE type, unsigned int slot);
	// Function to create constant buffer descriptor table.
	void CreateConstantBufferTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numOfConstants);
	// Function to create texture buffer descriptor table.
	void CreateTextureTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numOfTextures);
	// Function to create graphics pipeline.
	virtual void CreateGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);

public:
	// Function to call for setting the DXShader for render.
	void SetRenderDXShader(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Virtual functions to initialize the shader.
	virtual void Initilaize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples) {}
	virtual void Initialize(DXRenderEngine* renderEngine) {}
};