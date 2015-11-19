#include "system.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	System* runtimeObject;
	bool result;

	// Create the system object.
	runtimeObject = new System;
	if (!runtimeObject) return 0;

	// Initialize and run the system object.
	result = runtimeObject->Initialize();
	if (result)	runtimeObject->Run();

	// Shutdown and release the system object.
	runtimeObject->Shutdown();
	delete runtimeObject;
	runtimeObject = 0;

	return 0;
}