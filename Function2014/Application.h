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

	void ResizeScene(int width, int height);

	void RunMessageLoop();

private:
	HRESULT InitializeDirect3D();
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
	ComPtr<ID3D11Device2>           d3dDevice;
	ComPtr<ID3D11DeviceContext2>    d3dContext;
	ComPtr<IDXGISwapChain1>         swapChain;
	ComPtr<ID3D11RenderTargetView>  renderTargetView;
	ComPtr<ID3D11DepthStencilView>  depthStencilView;
	ComPtr<ID3D11BlendState>        blendState;
	ComPtr<ID3D11SamplerState>      samplerState;

	D3D11_VIEWPORT                  vp;
	D3D_FEATURE_LEVEL               d3dFeatureLevel;
	FLOAT                           viewportWidth;
	FLOAT                           viewportHeight;
	unique_ptr<IGeometryFactory>    geometryFactory;
	UINT                            backBufferCount;
	UINT                            renderTargetCount;

	unique_ptr<Timeline>            mainTimeline;
};
