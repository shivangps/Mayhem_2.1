#include "GenericShader.h"

namespace GenericShader
{
	void Shader::Initilaize(Microsoft::WRL::ComPtr<ID3D12Device5> device, DXGI_FORMAT renderTargetFormat, DXGI_FORMAT depthStencilFormat)
	{
		this->CreateDXShaderFromFile("Shaders/Generic/GenericVertexShader.hlsl", "Shaders/Generic/GenericPixelShader.hlsl");

		this->CreateConstantBufferTable(device, 1);
		this->CreateConstantBufferTable(device, 1);
		this->CreateTextureTable(device, 1);

		this->CreateGraphicsPipelineState(device, renderTargetFormat, depthStencilFormat);
	}

}
