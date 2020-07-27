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

	// It is a single ton class to generic shader.
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
		void Initilaize(Microsoft::WRL::ComPtr<ID3D12Device5> device, DXGI_FORMAT renderTargetFormat, DXGI_FORMAT depthStencilFormat);
	};
}