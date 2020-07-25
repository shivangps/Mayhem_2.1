#pragma once
#include "Helper.h"

// Class to handle the windows characteristics and getting keboard and mouse inputs.

class Win32Application
{
	// Class declared as a singleton pattern.
	Win32Application() {}
	~Win32Application() {}
public:
	static Win32Application* GetInstance()
	{
		static Win32Application* applicationInstance = new Win32Application();
		return applicationInstance;
	}
private:
	HWND hWnd = NULL;
	HINSTANCE hInstance = NULL;
	std::string window_title = "";
	std::wstring window_title_wide = L"";
	std::string window_class = "";
	std::wstring window_class_wide = L"";
	// Current render resoution.
	unsigned int width = 100;
	unsigned int height = 100;
	// Resolution dimensions when windowed.
	unsigned int windowed_width = 100;
	unsigned int windowed_height = 100;
	// Resolution dimensions when fullscreen.
	unsigned int fullscreen_width = 100;
	unsigned int fullscreen_height = 100;
	// Position of windows.
	unsigned int window_position_x = 250;
	unsigned int window_position_y = 100;

	// To check is window is to be closed or not.
	bool openWindow = true;
	bool fullscreenMode = true;
public:

	// Function to initialize the windows display and few other windows parameters such as windows title cursor and such...
	void Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, unsigned int width, unsigned int height);
	// Function for registering of a new window.
	void RegisteringOfWindow();
	// Function to process the message.
	void ProcessMessage();
	// Get Function for HWND.
	HWND GetWindowHandle();
	// Get Functions for current width and height.
	unsigned int GetWidth();
	unsigned int GetHeight();

	// Get function for fullscreen mode.
	bool IsWindowFullscreen();
	// Function to create a fullscreen.
	void CreateFullscreenMode();
	// Function to create a windowed screen.
	void CreateWindowedMode();

	// Get function for openWindow.
	bool IsWindowOpen();
	// Function to close the window.
	void CloseWindow();
};