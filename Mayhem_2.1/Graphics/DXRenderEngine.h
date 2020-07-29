#pragma once

#include "ADXRenderComponent.h"

// This header contains a class that is responsible for rendering all the game objects using DirectX 12 API.

// Class to render game objects using DirectX 12.
class DXRenderEngine
{
	// Class declared as a singleton pattern.
	DXRenderEngine() {}
	~DXRenderEngine()
	{
		this->device->Release();
		this->pAdapter->Release();
		this->factory->Release();
	}

public:
	static DXRenderEngine* GetInstance()
	{
		static DXRenderEngine* instance = new DXRenderEngine();
		return instance;
	}

private:
	HWND hWnd;

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter = nullptr;
	static Microsoft::WRL::ComPtr<ID3D12Device5> device;

	D3D12_COMMAND_LIST_TYPE mainCommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> mainCommandList;

	DXGI_FORMAT renderOutputFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const unsigned int frameCount = 2;		// Double buffering.
	unsigned int currentFrame = 0;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain = nullptr;

	DXDescriptorHeap rtvDescriptorHeap = {};
	DXResource backBuffer[frameCount];

	bool msaaEnable = true;
	DXDescriptorHeap msaaHeap = {};
	DXResource msaaResource = {};

	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	unsigned long long fenceValue = 0;
	HANDLE fenceEvent = NULL;

	D3D12_VIEWPORT viewport = {};
	D3D12_RECT scissorRect = {};

	DXDescriptorHeap depthStencilHeap = {};
	DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DXResource depthStencilResource = {};

	DXMeshSystem meshSystem;
	DXTextureSystem textureSystem;

private:
	// Function to create directx device factory.
	void CreateDeviceFactory();
	// Function to select the graphics card adapter.
	void SelectAdapter(unsigned int adapterIndex);
	// Function to create device.
	void CreateDevice();
	// Function to create the graphics command queue.
	void CreateCommandQueue();
	// Function to describe and create swap chain(aka. Render Double Buffer).
	void CreateSwapChain(unsigned int width, unsigned int height);
	// Function to create the render target view for the swap chain.
	void CreateRenderTargetView();
	// Function to create MSAA(Anti-Aliasing) render target buffer.
	void CreateMSAABuffer(unsigned int width, unsigned int height);
	// Function to create Depth/Stencil buffer.
	void CreateDepthStencilBuffer(unsigned int width, unsigned int height);
	// Function to create command allocator.
	void CreateCommandAllocator();
	// Function to create command list.
	void CreateGraphicsCommandList();
	// Function to create fence and CPU-GPU synchronization parameters.
	void CreateFenceNSynchronization();
	// Function to signal the fence value and then increament it.
	void SignalFence();
	// Function to present the frame on window screen.
	void PresentFrame();

	// XX------ EXPERIMENTAL - TO BE DELETED -------XX 
	ADXRenderComponent renderComponent;
	void PREP_RENDER_COMPONENT()
	{
#ifdef _DEBUG
		renderComponent.Initialize(this->device, this->mainCommandList, this->renderOutputFormat, this->depthStencilFormat, this->GetMeshSystem(), this->GetTextureSystem());
#endif // _DEBUG
	}
	void UPDATE_RENDER_COMPONENT()
	{
#ifdef _DEBUG
		renderComponent.Update(this->mainCommandList);
		this->meshSystem.DrawMesh(renderComponent.GetMeshIndex(), 1, this->mainCommandList);
#endif // _DEBUG
	}

public:
	// Function to initialize DirectX.
	void Initialize(HWND hWnd, unsigned int width, unsigned int height);
	// Function to reset swap chain with new resolution.
	void SetNewResolution(unsigned int width, unsigned int height);
	// Function to wait for GPU to finish executing previous frame command in the queue.
	void WaitForCommandExecution();
	// Function to flush all the commands in the command queue to start executing.
	void FlushCommands();
	// Function to uptade some elements of the DirectX graphics engine.
	void UpdateGraphicsState();
	// Function that contains the main rendering code.
	void RenderOnScreen();
	// Function to enable and disable MSAA.
	void EnableMSAA();
	void DisableMSAA();

public:
	// Some Get and Set functions.
	static Microsoft::WRL::ComPtr<ID3D12Device5> GetDirectXDevice();
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> GetMainCommandList();
	DXMeshSystem* GetMeshSystem();
	DXTextureSystem* GetTextureSystem();
};