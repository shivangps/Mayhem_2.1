#pragma once
#include "../Graphics/DXShader.h"

// A post processing shader that applies lighting to objects on G-buffer.

namespace SimpleLight
{
	typedef
		enum Slot
	{
		FragPosTex,
		ColorTex,
		NormalTex,
		SpecGlossTex,
		DepthTex,
		Size
	}Slot;

	// It is a singleton class to simple light shader.
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
		void Initilaize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
		// Function to create graphics pipeline.
		void CreateGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
	};
}