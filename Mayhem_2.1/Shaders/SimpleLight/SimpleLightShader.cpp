#include "SimpleLightShader.h"

// Header to initialize a simple lighting shader.

namespace SimpleLight
{
	void Shader::Initilaize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples)
	{
		this->CreateDXShaderFromFile("Shaders/SimpleLight/SimpleLightVertexShader.hlsl", "Shaders/SimpleLight/SimpleLightPixelShader.hlsl");

		this->CreateTextureTable(device, Slot::Size);

		this->CreateGraphicsPipelineState(device, numRT, renderTargetFormats, depthStencilFormat, samples);
	}

	void Shader::CreateGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples)
	{
		HRESULT HR;

		// Describe the rasterization process.
		D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

		// Describe the pipeline state.
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
		ZeroMemory(&pipelineDesc, sizeof(pipelineDesc));
		pipelineDesc.InputLayout = { DXQuadVertex::GetVertexLayout(), DXQuadVertex::elements };
		pipelineDesc.pRootSignature = rootSignature.Get();
		pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(vertexDXShader.Get());
		pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(pixelDXShader.Get());
		pipelineDesc.RasterizerState = rasterizerDesc;
		pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pipelineDesc.SampleMask = UINT_MAX;
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineDesc.NumRenderTargets = numRT;
		for (unsigned int i = 0; i < numRT; i++)
		{
			pipelineDesc.RTVFormats[i] = renderTargetFormats[i];
		}
		pipelineDesc.SampleDesc.Count = samples;
		pipelineDesc.SampleDesc.Quality = 0;
		pipelineDesc.DSVFormat = depthStencilFormat;

		HR = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(this->graphicsPipelineState.GetAddressOf()));
		CheckError(HR, "Failed to create graphics pipeline.");
	}
}