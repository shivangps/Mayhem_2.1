#include "DXRenderEngine.h"
#include <thread>

// Static variables.
Microsoft::WRL::ComPtr<ID3D12Device5> DXRenderEngine::device = nullptr;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> DXRenderEngine::mainCommandList = nullptr;

void DXRenderEngine::SETFRAMEBUFFERHEAP()
{
	this->framebufferheap.Initialize(this->device, 10, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	this->deferedRenderer.SetFragPosFramebufferToHandle(device, this->framebufferheap.GetCPUHandle(0));
	this->deferedRenderer.SetColorFramebufferToHandle(device, this->framebufferheap.GetCPUHandle(1));
	this->deferedRenderer.SetNormalFramebufferToHandle(device, this->framebufferheap.GetCPUHandle(2));
	this->deferedRenderer.SetSpecularFramebufferToHandle(device, this->framebufferheap.GetCPUHandle(3));
	this->deferedRenderer.SetDepthFramebufferToHandle(device, this->framebufferheap.GetCPUHandle(4));
}

// DirectX Graphics class functions.
void DXRenderEngine::Initialize(HWND hWnd, unsigned int width, unsigned int height)
{
	this->hWnd = hWnd;

	// Define a viewport.
	this->viewport.TopLeftX = 0.0f;
	this->viewport.TopLeftY = 0.0f;
	this->viewport.Width = width;
	this->viewport.Height = height;
	// viewport depth
	this->viewport.MinDepth = 0.0f;
	this->viewport.MaxDepth = 1.0f;

	this->scissorRect.left = 0;
	this->scissorRect.top = 0;
	this->scissorRect.right = width;
	this->scissorRect.bottom = height;

	this->CreateDeviceFactory();
	this->SelectAdapter(1);		// Selecting the RTX 2060 adapter, for my PC.
	this->CreateDevice();
	this->CreateCommandQueue();
	this->CreateSwapChain(width, height);
	this->CreateRenderTargetView();
	this->CreateDepthStencilView(width, height);
	this->CreateCommandAllocator();
	this->CreateGraphicsCommandList();
	this->CreateFenceNSynchronization();

	// EXPERIMENTAL: TO BE DELETED
	this->PREP_RENDER_COMPONENT();
	// Multithreading tests.
	std::vector<std::thread> t;
	t.push_back(std::thread(&DXRenderEngine::CallByThread, this));
	if (t[0].joinable())
		t[0].join();

	this->deferedRenderer.Initialize(this->device, width, height, (this->msaaEnable) ? this->maxMsaaVal : this->minMsaaVal);
	this->ppShader = SimpleLight::Shader::GetInstance();
	ppShader->Initilaize(this->device, 1, &this->renderOutputFormat, this->depthStencilFormat, 1);
	this->quadMesh = DXQuadMesh::GetInstance();
	this->quadMesh->Initialize(this->device);
	this->SETFRAMEBUFFERHEAP();

	this->mainCamera = MainCamera::GetInstance();
	this->mainCamera->windowWidth = width;
	this->mainCamera->windowHeight = height;

	this->cameraTransform = new Transform();
	this->cameraTransform->SetPosition(0.0f, 0.0f, 10.0f);
	this->cameraTransform->SetRotation(0.0f, 0.0f, 0.0f);
	Camera* newCamera = new Camera(cameraTransform, this->mainCamera->GetWindowWidth(), this->mainCamera->GetWindowHeight());
	newCamera->SetNearClip(0.01f);
	newCamera->SetFarClip(1000.0f);
	newCamera->SetFOV(45.0f);
	mainCamera->currentMainCamera = newCamera;

	// Flush all the commands after all initialization.
	this->FlushCommands();
}

void DXRenderEngine::SetNewResolution(unsigned int width, unsigned int height)
{
	this->swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	this->CreateRenderTargetView();
}

void DXRenderEngine::CreateDeviceFactory()
{
	HRESULT HR;

	unsigned int FactoryFlags = 0;

#if defined (_DEBUG)
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();

			FactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	HR = CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(this->factory.GetAddressOf()));
	CheckError(HR, "Failed to create device factory.");
}

void DXRenderEngine::SelectAdapter(unsigned int adapterIndex)
{
	HRESULT HR;

	// Getting adapter details.
	DXGI_ADAPTER_DESC adapterDesc = {};
	HR = this->factory->EnumAdapters1(adapterIndex, &this->pAdapter);
	pAdapter->GetDesc(&adapterDesc);
	CheckError(HR, "Failed to retrieve the detail/s of adapters.");
}

void DXRenderEngine::CreateDevice()
{
	HRESULT HR;

	//Create the device from the selected adapter.
	HR = D3D12CreateDevice(this->pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(this->device.GetAddressOf()));
	CheckError(HR, "Failed to create a device.");
}

void DXRenderEngine::CreateCommandQueue()
{
	HRESULT HR;

	// Command queue creation.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = this->mainCommandListType;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	HR = this->device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(this->commandQueue.GetAddressOf()));
	CheckError(HR, "Failed to create command queue.");
}

void DXRenderEngine::CreateSwapChain(unsigned int width, unsigned int height)
{
	HRESULT HR;

	// Decribe the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = this->frameCount;
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = this->renderOutputFormat;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.SampleDesc.Count = 1;

	// Create the swap chain.
	Microsoft::WRL::ComPtr<IDXGISwapChain1> tSwapChain = nullptr;
	HR = this->factory->CreateSwapChainForHwnd(this->commandQueue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &tSwapChain);
	CheckError(HR, "Failed to create swap chain.");

	// Disable the fullscreen toggle feature.
	HR = this->factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	CheckError(HR, "Failed to disable fullscreen toggle.");

	HR = tSwapChain.As(&this->swapChain);
	CheckError(HR, "Failed to transfer swap chain.");

	this->currentFrame = this->swapChain->GetCurrentBackBufferIndex();
}

void DXRenderEngine::CreateRenderTargetView()
{
	HRESULT HR;

	// Create render target view descriptor heap.
	rtvDescriptorHeap.Initialize(this->device, this->frameCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	// Create render target view.
	for (unsigned int i = 0; i < this->frameCount; i++)
	{
		HR = this->swapChain->GetBuffer(i, IID_PPV_ARGS(this->backBuffer[i].GetAddressOf()));
		this->device->CreateRenderTargetView(this->backBuffer[i].Get(), nullptr, this->rtvDescriptorHeap.GetCPUHandle(i));
		this->backBuffer[i].SetName(L"Back Buffer " + i);
	}

}

void DXRenderEngine::CreateDepthStencilView(unsigned int width, unsigned int height)
{
	HRESULT HR;

	// Create depth stencil view descriptor heap.
	dsvDescriptorHeap.Initialize(this->device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	// Create depth stencil buffer for Z - buffer.

	// Description for clearing depth stencil view.
	D3D12_CLEAR_VALUE depthOptimizedClearValue;
	depthOptimizedClearValue.Format = this->depthStencilFormat;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	// Describe the depth stencil buffer.
	D3D12_RESOURCE_DESC depthStencilDesc = {};
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = this->depthStencilFormat;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// Create resource.
	this->depthStencilResource.InitializeAsDefault(this->device, &depthStencilDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue);

	// Create depth stencil view.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = this->depthStencilFormat;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.Texture2D.MipSlice = 0;

	// Create depth stencil view.
	this->device->CreateDepthStencilView(this->depthStencilResource.Get(), &dsvDesc, this->dsvDescriptorHeap.GetCPUHandle(0));
}

void DXRenderEngine::CreateCommandAllocator()
{
	HRESULT HR = this->device->CreateCommandAllocator(this->mainCommandListType, IID_PPV_ARGS(this->commandAllocator.GetAddressOf()));
	CheckError(HR, "Failed to create command allocator.");
}

void DXRenderEngine::CreateGraphicsCommandList()
{
	HRESULT HR = this->device->CreateCommandList(0, this->mainCommandListType, this->commandAllocator.Get(), nullptr, IID_PPV_ARGS(mainCommandList.GetAddressOf()));
	CheckError(HR, "Failed to create graphics command list.");
	this->mainCommandList->SetName(L"Main Command List");
}

void DXRenderEngine::CreateFenceNSynchronization()
{
	HRESULT HR;

	HR = this->device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(this->fence.GetAddressOf()));
	CheckError(HR, "Failed to create synchronization fence.");

	this->fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (this->fenceEvent == nullptr)
	{
		HR = HRESULT_FROM_WIN32(GetLastError());
		CheckError(HR, "Failed to create fence event.");
	}

	this->SignalFence();
	this->WaitForCommandExecution();
}

void DXRenderEngine::SignalFence()
{
	HRESULT HR;

	HR = this->commandQueue->Signal(this->fence.Get(), this->fenceValue);
	CheckError(HR, "Failed to signal the fence.");
}

void DXRenderEngine::WaitForCommandExecution()
{
	HRESULT HR;

	if (this->fence->GetCompletedValue() < this->fenceValue)
	{
		HR = this->fence->SetEventOnCompletion(this->fenceValue, this->fenceEvent);
		CheckError(HR, "Failed to fence event not completed.");
		WaitForSingleObject(this->fenceEvent, INFINITE);
	}
	// Uppend the fence value once the fence event is complete.
	this->fenceValue++;
}

void DXRenderEngine::FlushCommands()
{
	HRESULT HR;

	// Submit the commands to the queue to be executed in the GPU.
	HR = this->mainCommandList->Close();
	CheckError(HR, "Failed to close the graphics command list.");

	// Execute the graphics command lists.
	ID3D12CommandList* ppCommandLists[] = { this->mainCommandList.Get() };
	this->commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	this->SignalFence();
}

void DXRenderEngine::UpdateGraphicsState()
{
	this->mainCamera->UpdateCameraValues();
}

void DXRenderEngine::PresentFrame()
{
	// Wait for all the command execution to finish.
	this->WaitForCommandExecution();

	// After all the commands are executed then present.
	HRESULT HR = this->swapChain->Present(1, 0);
	CheckError(HR, "Failed to present the frame.");
	this->currentFrame = this->swapChain->GetCurrentBackBufferIndex();
}

void DXRenderEngine::RenderOnScreen()
{
	HRESULT HR;

	this->PresentFrame();

	// Reset the allocator and command list.
	HR = this->commandAllocator->Reset();
	CheckError(HR, "Failed to reset command allocator.");

	HR = this->mainCommandList->Reset(this->commandAllocator.Get(), nullptr);
	CheckError(HR, "Failed to reset command list.");

	// Set the viewport and scissor rect.
	this->mainCommandList->RSSetViewports(1, &this->viewport);
	this->mainCommandList->RSSetScissorRects(1, &this->scissorRect);

	// Get the current frame back buffer to render on.
	DXResource* currentRenderBuffer = &this->backBuffer[this->currentFrame];

	// Transition the state of back buffer to render target if MSAA is dsabled.
	currentRenderBuffer->TransitionTo(this->mainCommandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//////////////////////// ALL RENDERING COMMANDS MUST TAKE PLACE FROM HERE ON ////////////////////////////
	
	// Get render target CPU handle appropriately based on MSAA being enabled.
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = {};	//C
	rtvHandle = this->rtvDescriptorHeap.GetCPUHandle(this->currentFrame);	//C
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = {};	//C
	dsvHandle = this->dsvDescriptorHeap.GetCPUHandle(0);	//C

	// Clear the render buffer with a specific color/value.
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	//C
	this->mainCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);	//C

	// Clear depth stencil buffer.
	this->mainCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);	//C

	// Setting up the framebuffer.
	this->deferedRenderer.SetFramebuffer(this->mainCommandList);	//C

	this->UPDATE_RENDER_COMPONENT();	//C

	// Removing the framebuffer.
	this->deferedRenderer.RemoveFramebuffer(this->mainCommandList, 1, &rtvHandle, &dsvHandle);	//C

	// Setup the post processing shader.
	this->ppShader->SetRenderDXShader(this->mainCommandList);	//C

	ID3D12DescriptorHeap* ppHeaps[] = { this->framebufferheap.Get() };	//C
	this->mainCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);	//C

	this->mainCommandList->SetGraphicsRootDescriptorTable(SimpleLight::FragPosTex, this->framebufferheap.GetGPUHandle(0));	//C
	this->mainCommandList->SetGraphicsRootDescriptorTable(SimpleLight::ColorTex, this->framebufferheap.GetGPUHandle(1));	//C
	this->mainCommandList->SetGraphicsRootDescriptorTable(SimpleLight::NormalTex, this->framebufferheap.GetGPUHandle(2));	//C
	this->mainCommandList->SetGraphicsRootDescriptorTable(SimpleLight::SpecGlossTex, this->framebufferheap.GetGPUHandle(3));	//C
	this->mainCommandList->SetGraphicsRootDescriptorTable(SimpleLight::DepthTex, this->framebufferheap.GetGPUHandle(4));	//C

	this->quadMesh->DrawQuad(this->mainCommandList);	//C

	///////////////////////////////// RENDERING COMMANDS ENDS HERE //////////////////////////////////////////

	// Transition the state of back buffer to render target if MSAA is dsabled.
	// Otherwise transfer the content from MSAA buffer to current back buffer.
	currentRenderBuffer->TransitionTo(this->mainCommandList, D3D12_RESOURCE_STATE_PRESENT);

	this->FlushCommands();
}

void DXRenderEngine::EnableMSAA()
{
	this->msaaEnable = true;
}

void DXRenderEngine::DisableMSAA()
{
	this->msaaEnable = false;
}

Microsoft::WRL::ComPtr<ID3D12Device5> DXRenderEngine::GetDirectXDevice()
{
	return device;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> DXRenderEngine::GetMainCommandList()
{
	return mainCommandList;
}

DXMeshSystem* DXRenderEngine::GetMeshSystem()
{
	return &this->meshSystem;
}

DXTextureSystem* DXRenderEngine::GetTextureSystem()
{
	return &this->textureSystem;
}
