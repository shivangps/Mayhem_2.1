#include "DXFramebuffer.h"

void DXFramebuffer::InitializeAsRenderbuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device, DXGI_FORMAT format, unsigned int width, unsigned int height, unsigned int multiSamples, std::wstring framebufferName)
{
	this->isDepthbuffer = false;
	this->renderFormat = format;

	// Description of render target resource.
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
	resourceDesc.Format = this->renderFormat;

	// Describe the clear value,
	D3D12_CLEAR_VALUE clearVal;
	clearVal.Color[0] = 0.0f;
	clearVal.Color[1] = 0.0f;
	clearVal.Color[2] = 0.0f;
	clearVal.Color[3] = 1.0f;
	clearVal.Format = this->renderFormat;

	this->renderBuffer.InitializeAsDefault(device, &resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearVal);
	this->renderBuffer.SetName((framebufferName + L" Render Buffer").c_str());

	// Describe the render target view.
	ZeroMemory(&this->rtvDesc, sizeof(this->rtvDesc));
	this->rtvDesc.Texture2D.MipSlice = 0;
	this->rtvDesc.Texture2D.PlaneSlice = 0;
	this->rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
	this->rtvDesc.Format = this->renderFormat;

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
	resourceDesc.Format = this->renderFormat;

	this->textureBuffer.InitializeAsDefault(device, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->renderBuffer.SetName((framebufferName + L" Shader Resource").c_str());

	// Describe the shader resource view.
	ZeroMemory(&this->srvDesc, sizeof(this->srvDesc));
	this->srvDesc.Texture2D.MipLevels = 1;
	this->srvDesc.Texture2D.MostDetailedMip = 0;
	this->srvDesc.Texture2D.PlaneSlice = 0;
	this->srvDesc.Format = this->renderFormat;
	this->srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	this->srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

void DXFramebuffer::InitializeAsDepthbuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height, unsigned int multiSamples, std::wstring depthbufferName)
{
	this->isDepthbuffer = true;

	// Describe the depth resource.
	D3D12_RESOURCE_DESC depthResourceDesc;
	ZeroMemory(&depthResourceDesc, sizeof(depthResourceDesc));
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Alignment = 0;
	depthResourceDesc.SampleDesc.Count = multiSamples;
	depthResourceDesc.SampleDesc.Quality = 0;
	depthResourceDesc.MipLevels = 1;
	depthResourceDesc.Format = this->depthRtFormat;
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Width = width;
	depthResourceDesc.Height = height;
	depthResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearVal = { this->depthRtFormat, 1.0f };

	// Create the render buffer.
	this->renderBuffer.InitializeAsDefault(device, &depthResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearVal);
	this->renderBuffer.SetName((depthbufferName + L" Render Buffer").c_str());

	// Create the shader resource.
	depthResourceDesc.SampleDesc.Count = 1;
	this->textureBuffer.InitializeAsDefault(device, &depthResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, &clearVal);
	this->renderBuffer.SetName((depthbufferName + L" Shader Resource").c_str());

	// Describe the depth resource view.
	ZeroMemory(&this->dsvDesc, sizeof(this->dsvDesc));
	this->dsvDesc.Texture2D.MipSlice = 0;
	this->dsvDesc.Format = this->depthRtFormat;
	this->dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	this->dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	// Describe the shader resource.
	ZeroMemory(&this->srvDesc, sizeof(this->srvDesc));
	this->srvDesc.Texture2D.MipLevels = 1;
	this->srvDesc.Texture2D.MostDetailedMip = 0;
	this->srvDesc.Format = this->depthSrFormat;
	this->srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
	this->srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

void DXFramebuffer::SetFramebufferToRTVHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	if (!this->isDepthbuffer)
	{
		device->CreateRenderTargetView(this->renderBuffer.Get(), &this->rtvDesc, handle);
	}
	else
	{
		ErrorLog("Invalid CPU handle assignment, framebuffer declared as depth buffer not depth buffer.");
		exit(-1);
	}
}

void DXFramebuffer::SetFramebufferToDSVHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	if (isDepthbuffer)
	{
		device->CreateDepthStencilView(this->renderBuffer.Get(), &this->dsvDesc, handle);
	}
	else
	{
		ErrorLog("Invalid CPU handle assignment, framebuffer declared as depth buffer not render buffer.");
		exit(-1);
	}
}

void DXFramebuffer::SetFramebufferToSRVHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	device->CreateShaderResourceView(this->textureBuffer.Get(), &this->srvDesc, handle);
}

void DXFramebuffer::CopyResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	TransferResourceContent(commandList, &this->renderBuffer, &this->textureBuffer, (this->isDepthbuffer) ? this->depthSrFormat : this->renderFormat);
}