#include "GameEngine.h"

void GameEngine::EnableFullscreen()
{
	if (!this->fullscreenMode)
	{
		// Tell the win32 application to go to fullscreen mode.
		this->winApplication->CreateFullscreenMode();
		// Change the resolution of render to fullscreen size.
		this->renderEngine->SetNewResolution(this->winApplication->GetWidth(), this->winApplication->GetHeight());
		this->fullscreenMode = true;
	}
}

void GameEngine::Initialize(HINSTANCE hInstance)
{
	unsigned int setWidth = 1280;
	unsigned int setHeight = 720;
	// Initialize a new window.
	this->winApplication->Initialize(hInstance, "Main Window", "EngineWindow", setWidth, setHeight);
	// Get fullscreen state.
	this->fullscreenMode = this->winApplication->IsWindowFullscreen();
	// Initialize the DirectX rendering engine and providing the created window handle.
	this->renderEngine->Initialize(this->winApplication->GetWindowHandle(), this->winApplication->GetWidth(), this->winApplication->GetHeight());
	// Start the time from start of the game.
	this->time = Time::GetInstance();
	this->time->Start();
}

void GameEngine::GameLoop()
{
	// Keep running till window closes.
	while (this->winApplication->IsWindowOpen())
	{
		// Process all the input messages.
		this->winApplication->ProcessMessage();

		// Reset the frame time.
		this->time->ResetFrameTime();

		// Render all the objects to the screen.
		this->renderEngine->RenderOnScreen();
	}
}