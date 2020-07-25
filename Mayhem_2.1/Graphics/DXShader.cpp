#include "DXShader.h"

void DXShader::CreateDXShaderFromFile(std::string vertexFilePath, std::string pixelFilePath)
{
	HRESULT HR;

#if defined(_DEBUG)
	// Enable better DXShader debugging with the graphics debugging tools.
	unsigned int compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	unsigned int compileFlags = 0;
#endif

	std::wstring fileName_wide;
	fileName_wide = StringToWide(vertexFilePath);
	HR = D3DCompileFromFile(fileName_wide.c_str(), nullptr, nullptr, "main", "vs_5_0", compileFlags, 0, &this->vertexDXShader, nullptr);
	CheckError(HR, "Failed to create vertex DXShader from file.");

	fileName_wide = StringToWide(pixelFilePath);
	HR = D3DCompileFromFile(fileName_wide.c_str(), nullptr, nullptr, "main", "ps_5_0", compileFlags, 0, &this->pixelDXShader, nullptr);
	CheckError(HR, "Failed to create pixel DXShader from file.");
}

void DXShader::CreateDefaultSampler()
{
	this->sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	this->sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	this->sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	this->sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	this->sampler.MipLODBias = 10;
	this->sampler.MaxAnisotropy = 0;
	this->sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	this->sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	this->sampler.MinLOD = 0.0f;
	this->sampler.MaxLOD = D3D12_FLOAT32_MAX;
	this->sampler.ShaderRegister = 0;
	this->sampler.RegisterSpace = 0;
	this->sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
}

void DXShader::CreateRootSignature(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	HRESULT HR;

	this->rootSigDesc.Init_1_1(this->rootParameters.size(), &rootParameters[0], 1, &this->sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;
	HR = D3DX12SerializeVersionedRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, &signature, &error);
	CheckError(HR, "Failed to serialize root signature.");

	HR = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(this->rootSignature.GetAddressOf()));
	CheckError(HR, "Failed to create the root signature.");

	signature->Release();
}

void DXShader::CreateDescriptorTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_DESCRIPTOR_RANGE_TYPE type, unsigned int slot)
{
	CD3DX12_DESCRIPTOR_RANGE1* pRange;
	pRange = new CD3DX12_DESCRIPTOR_RANGE1;
	pRange->Init(type, 1, slot, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	CD3DX12_ROOT_PARAMETER1 rootParameter;
	rootParameter.InitAsDescriptorTable(1, pRange, D3D12_SHADER_VISIBILITY_ALL);
	this->rootParameters.push_back(rootParameter);
	this->CreateRootSignature(device);
}

void DXShader::CreateConstantBufferTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numOfConstants)
{
	for (unsigned int i = 0; i < numOfConstants; i++)
		this->CreateDescriptorTable(device, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, i);
}

void DXShader::CreateTextureTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numOfTextures)
{
	for (unsigned int i = 0; i < numOfTextures; i++)
		this->CreateDescriptorTable(device, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, i);
}

void DXShader::CreateGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12Device5> device, DXGI_FORMAT renderTargetFormat, DXGI_FORMAT depthStencilFormat)
{
	HRESULT HR;

	// Describe the rasterization process.
	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	// Describe the pipeline state.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
	ZeroMemory(&pipelineDesc, sizeof(pipelineDesc));
	pipelineDesc.InputLayout = { DXVertex::GetVertexLayout(), DXVertex::elements };
	pipelineDesc.pRootSignature = rootSignature.Get();
	pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(vertexDXShader.Get());
	pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(pixelDXShader.Get());
	pipelineDesc.RasterizerState = rasterizerDesc;
	pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineDesc.SampleMask = UINT_MAX;
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = renderTargetFormat;
	pipelineDesc.SampleDesc.Count = 4;
	pipelineDesc.SampleDesc.Quality = 0;
	pipelineDesc.DSVFormat = depthStencilFormat;

	HR = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(this->graphicsPipelineState.GetAddressOf()));
	CheckError(HR, "Failed to create graphics pipeline.");
}

void DXShader::SetRenderDXShader(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	commandList->SetPipelineState(this->graphicsPipelineState.Get());
	commandList->SetGraphicsRootSignature(this->rootSignature.Get());
}
