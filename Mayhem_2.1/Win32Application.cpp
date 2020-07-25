#include "Win32Application.h"

void Win32Application::CreateFullscreenMode()
{
	// Apply fullscreen.
	SetWindowLongPtr(this->hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
	SetWindowPos(this->hWnd, HWND_TOP, 0, 0, this->fullscreen_width, this->fullscreen_height, SWP_FRAMECHANGED);
	// Assign new current resolution to render.
	this->width = this->fullscreen_width;
	this->height = this->fullscreen_height;
	
	this->fullscreenMode = true;
}

void Win32Application::CreateWindowedMode()
{
	// Apply windowed.
	SetWindowLongPtr(this->hWnd, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
	SetWindowPos(this->hWnd, NULL, this->window_position_x, this->window_position_y, this->windowed_width, this->windowed_height, SWP_FRAMECHANGED);
	// Assign new current resolution to render.
	this->width = this->windowed_width;
	this->height = this->windowed_height;

	this->fullscreenMode = false;
}

void Win32Application::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, unsigned int windowed_width, unsigned int windowed_height)
{
	// Get the screen dimensions for fullscreen.
	this->fullscreen_width = GetSystemMetrics(SM_CXSCREEN);
	this->fullscreen_height = GetSystemMetrics(SM_CYSCREEN);
	// Assign windowed resolution.
	this->windowed_width = windowed_width;
	this->windowed_height = windowed_height;

	// Initializing the window first in fullscreen mode.
	this->hInstance = hInstance;
	// Assign current resolution.
	this->width = this->fullscreen_width;
	this->height = this->fullscreen_height;

	this->window_class = window_class;
	this->window_class_wide = StringToWide(window_class);
	this->window_title = window_title;
	this->window_title_wide = StringToWide(window_title);

	RegisteringOfWindow();

	this->hWnd = CreateWindowEx(
		0,
		this->window_class_wide.c_str(),
		this->window_title_wide.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE | WS_POPUP,
		0,
		0,
		this->width,
		this->height,
		NULL,
		NULL,
		this->hInstance,
		nullptr);
	// Sanity check for window creation.
	if (this->hWnd == NULL)
	{
		ErrorLog("Windows creation failed for windows: " + window_title);
		exit(-1);
	}
	// Select the created window for current use in front of all windows opened.
	ShowWindow(this->hWnd, SW_SHOW);
	SetForegroundWindow(this->hWnd);
	SetFocus(this->hWnd);

	// Complete fullscreen mode.
	CreateFullscreenMode();

	return;
}

// Function to handle the windows input messages.
LRESULT CALLBACK HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Win32Application* application = Win32Application::GetInstance();

	switch (uMsg)
	{
		// Keyboard messages.
		case WM_KEYDOWN:
		{
			// Exit the application if escape key is pressed.
			if (wParam == VK_ESCAPE)
			{
				application->CloseWindow();
			}
		}
		return 0;
		// If close button is pressed then simply close the window.
		case WM_CLOSE:
		{
			application->CloseWindow();
		}
		return 0;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

}

void Win32Application::RegisteringOfWindow()
{
	WNDCLASSEX wc;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = HandleMessage;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->hInstance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->window_class_wide.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassExW(&wc))
	{
		ErrorLog("Windows Registration Failed!");
		exit(-1);
	}
	return;
}

void Win32Application::ProcessMessage()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	if (PeekMessage(&msg, this->hWnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL)
	{
		if (!IsWindow)
		{
			this->hWnd;
			this->openWindow = false;
			UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
			return;
		}
	}

	return;
}

HWND Win32Application::GetWindowHandle()
{
	return this->hWnd;
}

unsigned int Win32Application::GetWidth()
{
	return this->width;
}

unsigned int Win32Application::GetHeight()
{
	return this->height;
}

bool Win32Application::IsWindowOpen()
{
	return this->openWindow;
}

void Win32Application::CloseWindow()
{
	this->openWindow = false;
}

bool Win32Application::IsWindowFullscreen()
{
	return this->fullscreenMode;
}
