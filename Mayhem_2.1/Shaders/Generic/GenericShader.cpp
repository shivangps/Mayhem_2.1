#include "GenericShader.h"

namespace GenericShader
{
	void Shader::Initilaize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples)
	{
		this->CreateDXShaderFromFile("Shaders/Generic/GenericVertexShader.hlsl", "Shaders/Generic/GenericPixelShader.hlsl");

		this->CreateConstantBufferTable(device, 1);
		this->CreateConstantBufferTable(device, 1);
		this->CreateTextureTable(device, 1);

		this->CreateGraphicsPipelineState(device, numRT, renderTargetFormats, depthStencilFormat, samples);
	}

}
