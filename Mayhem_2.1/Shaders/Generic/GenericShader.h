#pragma once
#include "../Graphics/DXShader.h"

namespace DXShaders
{
	// It is a single ton class to generic shader.
	class GenericShader : public DXShader
	{
		GenericShader() {}

	public:
		// Function to get a single instance of the class.
		static GenericShader* GetInstance()
		{
			static GenericShader* instance = new GenericShader();
			return instance;
		}

		// Override of virtual function to initialize.
		void Initilaize(Microsoft::WRL::ComPtr<ID3D12Device5> device, DXGI_FORMAT renderTargetFormat, DXGI_FORMAT depthStencilFormat);
	};
}