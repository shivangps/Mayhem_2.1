#include "ADXRenderComponent.h"

void ADXRenderComponent::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList,
	DXGI_FORMAT renderTargetFormat, DXGI_FORMAT depthStencilFormat, DXMeshSystem* meshSysytem)
{
	this->application = Win32Application::GetInstance();

	this->shader = DXShaders::GenericShader::GetInstance();
	this->shader->Initilaize(device, renderTargetFormat, depthStencilFormat);

	this->InitializeBuffers(device, commandList);
	this->SetMeshIndex(meshSysytem->RegisterModel(device, commandList, "3DObjects/metaCube.obj"));
}

void ADXRenderComponent::InitializeBuffers(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	HRESULT HR;

	//1. CREATE CBV & SRV DESCRIPTOR HEAP
	//2. CREATE CONSTANT BUFFER VIEW
	//3. CREATE SHADER RESOURCE VIEW(FOR TEXTURE)

	// Create CBV(constant buffer view) & SRV(shader resource view) descriptor heap.
	this->cbv_srv_heap.Initialize(device, 3, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	// Create constant buffer view for saturation value(float).
	{
		this->saturation_constant_resource.InitializeAsUpload(device, &CD3DX12_RESOURCE_DESC::Buffer(GetAggregateSize(sizeof(SatCBuffer))), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
		this->saturation_constant_resource.SetName(L"Saturation Buffer");

		D3D12_CONSTANT_BUFFER_VIEW_DESC satCBViewDesc = {};
		satCBViewDesc.BufferLocation = this->saturation_constant_resource.GetGPUVirtualAddress();
		satCBViewDesc.SizeInBytes = GetAggregateSize(sizeof(SatCBuffer));

		device->CreateConstantBufferView(&satCBViewDesc, this->cbv_srv_heap.GetCPUHandle(0));

		CD3DX12_RANGE readRange = {};
		this->saturation_constant_resource.Map(0, &readRange, reinterpret_cast<void**>(&p_sat_cbv_begin));
		this->saturation_data.saturation = 0.5f;
		memcpy(p_sat_cbv_begin, &this->saturation_data, sizeof(this->saturation_data));
		this->saturation_constant_resource.Unmap(0, nullptr);
	}

	// Create constant buffer view for transformation matrix.
	{
		this->matrix_constant_resource.InitializeAsUpload(device, &CD3DX12_RESOURCE_DESC::Buffer(GetAggregateSize(sizeof(MatCBuffer))), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
		this->matrix_constant_resource.SetName(L"Transformation Matrix Buffer");

		D3D12_CONSTANT_BUFFER_VIEW_DESC matCBViewDesc = {};
		matCBViewDesc.BufferLocation = this->matrix_constant_resource.GetGPUVirtualAddress();
		matCBViewDesc.SizeInBytes = GetAggregateSize(sizeof(MatCBuffer));

		device->CreateConstantBufferView(&matCBViewDesc, this->cbv_srv_heap.GetCPUHandle(1));

		CD3DX12_RANGE readRange = {};
		this->matrix_constant_resource.Map(0, &readRange, reinterpret_cast<void**>(&this->p_mat_cbv_begin));
		DirectX::XMMATRIX identityMatrix = DirectX::XMMatrixIdentity();
		DirectX::XMStoreFloat4x4(&this->matrix_data.TransformationMatrix, identityMatrix);
		DirectX::XMStoreFloat4x4(&this->matrix_data.ModelMatrix, identityMatrix);
		DirectX::XMStoreFloat4x4(&this->matrix_data.InvTrpModelMatrix, identityMatrix);
		memcpy(this->p_mat_cbv_begin, &this->matrix_data, sizeof(this->matrix_data));
		this->matrix_constant_resource.Unmap(0, nullptr);
	}

	// Create shader resource view for texture buffer.
	{
		texture.Initialize(device, commandList, "Textures/container2.png");
		texture.CreateResourceView(device, this->cbv_srv_heap.GetCPUHandle(2));
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
	DirectX::XMMATRIX model, view, projection;

	// MODEL
	model = DirectX::XMMatrixIdentity();
	model = DirectX::XMMatrixRotationZ(time->GetCurrentStartTime() / 1000.0f) * model;
	model = DirectX::XMMatrixRotationX(time->GetCurrentStartTime() / 1000.0f) * model;
	model = DirectX::XMMatrixRotationY(time->GetCurrentStartTime() / 1000.0f) * model;

	// VIEW
	DirectX::XMVECTOR cameraPosition = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f);
	DirectX::XMVECTOR targetLookPos = DirectX::XMVectorZero();
	DirectX::XMVECTOR upPosition = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	view = DirectX::XMMatrixLookAtLH(cameraPosition, targetLookPos, upPosition);

	// PROJECTION
	projection = DirectX::XMMatrixPerspectiveFovLH(45.0f, (float)this->application->GetWidth() / (float)this->application->GetHeight(), 0.1f, 100.0f);

	// MVP
	DirectX::XMMATRIX MVPmatrix = model * view * projection;

	DirectX::XMStoreFloat4x4(&this->matrix_data.TransformationMatrix, DirectX::XMMatrixTranspose(MVPmatrix));
	DirectX::XMStoreFloat4x4(&this->matrix_data.ModelMatrix, DirectX::XMMatrixTranspose(model));
	DirectX::XMStoreFloat4x4(&this->matrix_data.InvTrpModelMatrix, DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(model), model));
	memcpy(this->p_mat_cbv_begin, &this->matrix_data, sizeof(this->matrix_data));
}

void ADXRenderComponent::Update(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	this->UpdateState();

	this->shader->SetRenderDXShader(commandList);

	ID3D12DescriptorHeap* ppHeaps[] = { this->cbv_srv_heap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->SetGraphicsRootDescriptorTable(0, this->cbv_srv_heap.GetGPUHandle(0));
	commandList->SetGraphicsRootDescriptorTable(1, this->cbv_srv_heap.GetGPUHandle(1));
	commandList->SetGraphicsRootDescriptorTable(2, this->cbv_srv_heap.GetGPUHandle(2));

}

unsigned int ADXRenderComponent::GetMeshIndex()
{
	return this->meshIndex;
}

void ADXRenderComponent::SetMeshIndex(unsigned int index)
{
	this->meshIndex = index;
}
