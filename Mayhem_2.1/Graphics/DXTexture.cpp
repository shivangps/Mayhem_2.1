#include "DXTexture.h"

void DXTexture::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, std::string fileDirectory)
{
	HRESULT HR;

	// Create shader resource view for texture buffer.
	std::unique_ptr<unsigned char[]> decodedData;
	D3D12_SUBRESOURCE_DATA subresources;

	HR = DirectX::LoadWICTextureFromFile(device.Get(), StringToWide(fileDirectory).c_str(), this->mainTexture.GetAddressOf(), decodedData, subresources);
	CheckError(HR, "Failed to create texture from file: " + fileDirectory);
	this->mainTexture.SetName(StringToWide(GetFileNameFromDirectory(fileDirectory + " Resource")).c_str());

	const unsigned long long uploadBufferSize = GetRequiredIntermediateSize(this->mainTexture.Get(), 0, 1);

	this->uploadTexture.InitializeAsUpload(device.Get(), &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);

	UpdateSubresources(commandList.Get(), this->mainTexture.Get(), this->uploadTexture.Get(), 0, 0, 1, &subresources);
	this->mainTexture.TransitionTo(commandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void DXTexture::CreateResourceView(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	device->CreateShaderResourceView(this->mainTexture.Get(), nullptr, handle);
}
