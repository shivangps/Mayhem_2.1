#pragma once
#include "Graphics/DXRenderEngine.h"


// Header that contains the game engine class to run the core game loop and is a collection of Input, Scene, Rendering Engine components
// as well as running main window display screen.
class GameEngine
{
	// Class declared as a singleton pattern.
	GameEngine() {}
	~GameEngine() {}

public:
	static GameEngine* GetInstance()
	{
		static GameEngine* instance = new GameEngine();
		return instance;
	}

private:
	// Boolean to kep track of fullscreen mode.
	bool fullscreenMode = true;
	// Win32 application to run the windows.
	Win32Application* winApplication = Win32Application::GetInstance();
	// Render engine.
	DXRenderEngine* renderEngine = DXRenderEngine::GetInstance();
	// Time.
	Time* time = nullptr;

private:
	// Function to implement fullscreen functionality if enabled.
	void EnableFullscreen();

public:
	// Game engine initialization.
	void Initialize(HINSTANCE hInstance);
	// Main game loop which will run the entire game.
	void GameLoop();
};