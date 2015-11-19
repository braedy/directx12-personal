#ifndef _SYSTEM_H_
#define _SYSTEM_H_

// preprocessing directive (removes less used win32 apis)
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "application.h"

class System{
	LPCWSTR m_applicationName;
	HINSTANCE m_hInstance;
	HWND m_hwnd;

	Application* m_Application;

	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

public:
	System();
	System(const System&);
	~System();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
};

// redirect messaging into MessageHandler
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global
static System* ApplicationHandle = 0;

#endif