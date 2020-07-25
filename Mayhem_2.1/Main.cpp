#include "GameEngine.h"

// Main windows application execution.

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{

	GameEngine* engine = GameEngine::GetInstance();
	engine->Initialize(hInstance);
	engine->GameLoop();

	return 0;
}