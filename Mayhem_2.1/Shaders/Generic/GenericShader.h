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
		void Initilaize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
	};
}