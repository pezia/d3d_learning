// include the basic windows header file
#include "Application.h"

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "DXGI.lib")

int WINAPI WinMain(
	_In_ HINSTANCE hInst, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	// Ignore the return value because we want to run the program even in the
	// unlikely event that HeapSetInformation fails.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			Application app;

			if (SUCCEEDED(app.Initialize()))
			{
				app.RunMessageLoop();
			}
			else
			{
				MessageBox(NULL, "Initialization failed", "Error", MB_ICONSTOP | MB_OK);
			}
		}
		CoUninitialize();
	}

	return 0;
}
