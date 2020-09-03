#pragma once
#include "../Graphics/DXShader.h"

namespace GenericShader
{
	typedef
		enum Slot
	{
		Saturation,
		Matrix,
		Texture
	} Slot;

	// It is a singleton class to generic shader.
	class Shader : public DXShader
	{
		Shader() {}

	public:
		// Function to get a single instance of the class.
		static Shader* GetInstance()
		{
			static Shader* instance = new Shader();
			return instance;
		}

		// Override of virtual function to initialize.
		void Initilaize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples)
		{
			this->CreateDXShaderFromFile("Shaders/Generic/GenericVertexShader.hlsl", "Shaders/Generic/GenericPixelShader.hlsl");

			this->CreateConstantBufferTable(device, 1);
			this->CreateConstantBufferTable(device, 1);
			this->CreateTextureTable(device, 1);

			this->CreateGraphicsPipelineState(device, numRT, renderTargetFormats, depthStencilFormat, samples);
		}
	};
}