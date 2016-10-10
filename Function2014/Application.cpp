#include "Application.h"
#include "common.h"

using namespace Microsoft::WRL;

Application::Application() :
	hWnd(NULL),
	geometryFactory(nullptr),
	backBufferCount(2),
	renderTargetCount(1),
	d3dResources(nullptr)
{

}

Application::~Application()
{
	mainTimeline = nullptr;
	geometryFactory = nullptr;
	d3dResources = nullptr;

	CloseWindow(hWnd);
	DestroyWindow(hWnd);

#ifdef _DEBUG
	// Dump debug info when exiting.
	DumpD3DDebug();
#endif //_DEGBUG
}

HRESULT Application::Initialize()
{
	HRESULT hr;

	// Register the window class.
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Application::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = "PeziaDemo";

	RegisterClassEx(&wcex);

	// Create the application window.
	int dpiX = 0;
	int dpiY = 0;
	HDC hdc = GetDC(NULL);

	if (hdc)
	{
		dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
		dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
		ReleaseDC(NULL, hdc);
	}

	hWnd = CreateWindow(
		"PeziaDemo",
		"Demo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
		static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
		NULL,
		NULL,
		HINST_THISCOMPONENT,
		this
	);

	hr = hWnd ? S_OK : E_FAIL;

	if (SUCCEEDED(hr))
	{
		ShowWindow(hWnd, SW_SHOWNORMAL);
		UpdateWindow(hWnd);
	}

	d3dResources = std::make_unique<D3DResources>(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT);

	d3dResources->setHwnd(hWnd);

	if (SUCCEEDED(hr))
	{
		hr = LoadDemo();
	}

	return hr;
}

HRESULT Application::LoadDemo()
{
	std::unique_ptr<IGeometryFactory> geometryFactoryPtr = make_unique<D3D11GeometryFactory>(d3dResources->d3dDevice.Get());

	DemoLoader loader(geometryFactoryPtr.get(), d3dResources->d3dDevice.Get());

	mainTimeline = unique_ptr<Timeline>(loader.loadDemoFromFile(std::wstring(L"")));

	return S_OK;
}

void Application::ResizeScene(UINT width, UINT height) {
	if (d3dResources != nullptr)
	{
		d3dResources->setSize(width, height);
	}
}

void Application::RunMessageLoop()
{
	MSG msg;

	bool done = false;

	Timer timer;

	ULONGLONG start = GetTickCount64();

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))			// Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)
			{
				done = true;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		Render(GetTickCount64() - start);

		timer.Tick();
	}
}


LRESULT CALLBACK Application::WndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
)
{
	Application* pApplication = nullptr;
	LPCREATESTRUCT pcs = nullptr;

	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return 0;
		}
		break;
	case WM_CREATE:
		pcs = (LPCREATESTRUCT)lParam;
		pApplication = (Application *)pcs->lpCreateParams;

		::SetWindowLongPtrW(hWnd, GWLP_USERDATA, PtrToUlong(pApplication));

		return 1;
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:
		pApplication = reinterpret_cast<Application*>(static_cast<LONG_PTR>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA)));
		pApplication->ResizeScene(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Application::Render(ULONGLONG tickCount)
{
	if (!d3dResources->swapChain || !d3dResources->renderTargetView)
	{
		return;
	}

	ID3D11RenderTargetView *const targets[1] = { d3dResources->renderTargetView.Get() };
	d3dResources->d3dContext->OMSetRenderTargets(d3dResources->renderTargetCount, targets, d3dResources->depthStencilView.Get());

	d3dResources->d3dContext->ClearRenderTargetView(d3dResources->renderTargetView.Get(), Colors::CornflowerBlue);
	d3dResources->d3dContext->ClearDepthStencilView(d3dResources->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if (mainTimeline)
	{
		mainTimeline->Render(tickCount, d3dResources.get());
	}

	d3dResources->swapChain->Present(0, 0);
}

#ifdef _DEBUG
void Application::DumpD3DDebug()
{
	// This is debug code to free up all allocated D3D resources, and generate debug output for leaked objects.
	// The way this works is that D3D keeps track of all objects. When we destroy the m_main and m_deviceResources classes, 
	// all D3D objects should be released. If any objects that D3D thinks are not released are being leaked and the code should be examined.
	
	// Get the debug interface for the device.
	ComPtr<IDXGIDebug1> debugInterface = nullptr;
	
	if (FAILED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debugInterface))))
	{
		return;
	}

	// Dump the list of leaked objects to the debugger output window.
	DirectxHelper::ThrowIfFailed(debugInterface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL));
}
#endif
