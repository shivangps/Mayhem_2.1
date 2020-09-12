#pragma once
#include "DXMeshSystem.h"
#include "../Shaders/ShaderCollectionHeader.h"
#include "DXTexture.h"
#include "../MathEngine.h"
#include "Camera.h"

struct GlobalCBuffer
{
	float ambientLight;
};

struct LocalCBuffer
{
	Matrix4 TransformationMatrix;
	Matrix4 ModelMatrix;
	Matrix4 ViewMatrix;
	Matrix4 NormalMatrix;
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
	GlobalCBuffer global_data = {};
	DXResource global_constant_resource = {};

	// object 1
	unsigned char* p_ob1_local_cbv_begin = nullptr;
	LocalCBuffer ob1_local_data = {};
	DXResource ob1_local_constant_resource = {};

	// object 2
	unsigned char* p_ob2_local_cbv_begin = nullptr;
	LocalCBuffer ob2_local_data = {};
	DXResource ob2_local_constant_resource = {};

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