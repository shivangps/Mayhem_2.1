#include "ADXRenderComponent.h"

void ADXRenderComponent::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList,
	unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, DXMeshSystem* meshSysytem, DXTextureSystem* textureSystem,
	 unsigned int samples)
{
	this->application = Win32Application::GetInstance();

	this->shader = GenericShader::Shader::GetInstance();
	this->shader->Initilaize(device, numRT, renderTargetFormats, depthStencilFormat, samples);

	this->InitializeBuffers(device, commandList, textureSystem);
	this->SetMeshIndex(meshSysytem->RegisterModel(device, commandList, "3DObjects/metaCube.obj"));
}

void ADXRenderComponent::InitializeBuffers(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, DXTextureSystem* textureSystem)
{
	//1. CREATE CBV & SRV DESCRIPTOR HEAP
	//2. CREATE CONSTANT BUFFER VIEW
	//3. CREATE SHADER RESOURCE VIEW(FOR TEXTURE)

	// Create CBV(constant buffer view) & SRV(shader resource view) descriptor heap.
	this->cbv_srv_heap.Initialize(device, 3, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	// Create constant buffer view for saturation value(float).
	{
		this->p_sat_cbv_begin = CreateConstantBuffer(device, &this->saturation_constant_resource, sizeof(SatCBuffer), this->cbv_srv_heap.GetCPUHandle(GenericShader::Saturation), L"Saturation Constant Buffer");
	}

	// Create constant buffer view for transformation matrix.
	{
		this->p_mat_cbv_begin = CreateConstantBuffer(device, &this->matrix_constant_resource, sizeof(MatCBuffer), this->cbv_srv_heap.GetCPUHandle(GenericShader::Matrix), L"Matrix Constant Buffer");
	}

	// Create shader resource view for texture buffer.
	{
		this->texture = textureSystem->RegisterTexture(device, commandList, "Textures/container2.png");
		textureSystem->CreateResourceView(this->texture, device, this->cbv_srv_heap.GetCPUHandle(GenericShader::Texture));
	}
	return;
}

void ADXRenderComponent::UpdateState()
{
	Time* time = Time::GetInstance();

	// Saturation constant buffer.
	this->saturation_data.saturation = 0.05f;
	memcpy(this->p_sat_cbv_begin, &saturation_data, sizeof(saturation_data));

	// Matrix constant buffer.
	Matrix4 model, view, projection;

	position = Vector3(1.0f, 0.0f, 0.0f);

	// MODEL
	model = model.Translation(position);
	model = model.Scale(Vector3(0.5f));
	model = DirectX::XMMatrixRotationX(time->GetCurrentStartTime() / 1000.0f) * model.GetMatrix();
	model = DirectX::XMMatrixRotationY(time->GetCurrentStartTime() / 1000.0f) * model.GetMatrix();

	// VIEW
	Vector3 cameraPosition(0.0f, 0.0f, -5.0f);
	Vector3 targetLookPos;
	Vector3 upPosition(0.0f, 1.0f, 0.0f);
	view = DirectX::XMMatrixLookAtLH(cameraPosition.GetVector(), targetLookPos.GetVector(), upPosition.GetVector());

	// PROJECTION
	projection = DirectX::XMMatrixPerspectiveFovLH(45.0f, (float)this->application->GetWidth() / (float)this->application->GetHeight(), 0.1f, 100.0f);

	// MVP
	Matrix4 mvpMatrix = model * view * projection;

	this->matrix_data.TransformationMatrix = mvpMatrix.Transpose();
	this->matrix_data.ModelMatrix = model;
	this->matrix_data.InvTrpModelMatrix = model.Inverse(model.Determinant());
	memcpy(this->p_mat_cbv_begin, &this->matrix_data, sizeof(this->matrix_data));
}

void ADXRenderComponent::Update(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, DXMeshSystem* meshSystem)
{
	this->UpdateState();

	this->shader->SetRenderDXShader(commandList);

	ID3D12DescriptorHeap* ppHeaps[] = { this->cbv_srv_heap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->SetGraphicsRootDescriptorTable(GenericShader::Saturation, this->cbv_srv_heap.GetGPUHandle(GenericShader::Saturation));
	commandList->SetGraphicsRootDescriptorTable(GenericShader::Matrix, this->cbv_srv_heap.GetGPUHandle(GenericShader::Matrix));
	commandList->SetGraphicsRootDescriptorTable(GenericShader::Texture, this->cbv_srv_heap.GetGPUHandle(GenericShader::Texture));

	meshSystem->DrawMesh(this->meshIndex, 1, commandList);
}

unsigned int ADXRenderComponent::GetMeshIndex()
{
	return this->meshIndex;
}

void ADXRenderComponent::SetMeshIndex(unsigned int index)
{
	this->meshIndex = index;
}
