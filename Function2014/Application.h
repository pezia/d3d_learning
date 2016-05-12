#pragma once

#include "common.h"
#include "classes.h"

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

using namespace Microsoft::WRL;
using namespace std;

class Application
{
public:
	Application();
	~Application();

	HRESULT Initialize();

	void ResizeScene(UINT width, UINT height);

	void RunMessageLoop();

private:
	HRESULT LoadDemo();
	void Render(DWORD tickCount);
	void DumpD3DDebug();

	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		);

private:
	HWND                            hWnd;
	unique_ptr<D3DResources>        d3dResources;
	FLOAT                           viewportWidth;
	FLOAT                           viewportHeight;
	unique_ptr<IGeometryFactory>    geometryFactory;
	UINT                            backBufferCount;
	UINT                            renderTargetCount;

	unique_ptr<Timeline>            mainTimeline;
};
