#pragma once
#include "DXMeshSystem.h"
#include "DXTextureSystem.h"
#include "../Shaders/ShaderCollectionHeader.h"
#include "DXTexture.h"
#include "../MathEngine.h"
#include "Camera.h"

struct SatCBuffer
{
	float saturation;
};

struct MatCBuffer
{
	Matrix4 TransformationMatrix;
	Matrix4 ModelMatrix;
	Matrix4 InvTrpModelMatrix;
};

class ADXRenderComponent
{
public:
	ADXRenderComponent() {}
	~ADXRenderComponent() {}

private:
	DXShader* shader = nullptr;

	unsigned int texture = 0;

	DXDescriptorHeap cbv_srv_heap = {};

	unsigned char* p_sat_cbv_begin = nullptr;
	SatCBuffer saturation_data = {};
	DXResource saturation_constant_resource = {};

	// object 1
	unsigned char* p_mat_cbv_begin = nullptr;
	MatCBuffer matrix_data = {};
	DXResource matrix_constant_resource = {};

	// object 2
	unsigned char* p_ob2_mat_cbv_begin = nullptr;
	MatCBuffer ob2_matrix_data = {};
	DXResource ob2_matrix_constant_resource = {};

	Win32Application* application = nullptr;

public:
	// Function to be called for initialization.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList,
		unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, DXMeshSystem* meshSystem, DXTextureSystem* textureSystem, unsigned int samples);
	// Function to initialize texture and constant buffers.
	void InitializeBuffers(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, DXTextureSystem* textureSystem);
	// Function to update constant buffers.
	void UpdateState();
	// Function to be called for updated drawing.
	void Update(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, DXMeshSystem* meshSystem);

private:
	// Stores which mesh from collection.
	unsigned int meshIndex = 0;
	// Object transform.
	Transform transform_object_1;
	Transform transform_object_2;

public:
	// Get/Set function for mesh index.
	unsigned int GetMeshIndex();
	void SetMeshIndex(unsigned int index);
};