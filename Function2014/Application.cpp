#include "Application.h"
#include "common.h"

using namespace Microsoft::WRL;

Application::Application() :
	hWnd(NULL),
	geometryFactory(nullptr),
	backBufferCount(2),
	renderTargetCount(1)
{
}

Application::~Application()
{
	if (swapChain)
	{
		swapChain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode
		ID3D11RenderTargetView* nullViews[] = { nullptr };
		d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	}

	if (d3dContext) {
		d3dContext->Flush();
	}

	mainTimeline = nullptr;
	geometryFactory = nullptr;

	blendState = nullptr;
	samplerState = nullptr;
	swapChain = nullptr;
	depthStencilView = nullptr;
	renderTargetView = nullptr;
	d3dContext = nullptr;
	d3dDevice = nullptr;

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

		// Initialize DirectComposition resources, such as the
		// device object and composition target object.
		hr = InitializeDirect3D();
	}

	if (SUCCEEDED(hr))
	{
		hr = LoadDemo();
	}

	return hr;
}

HRESULT Application::InitializeDirect3D()
{
	HRESULT hr;

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	UINT deviceFlags = 0;

	if (DirectxHelper::SdkLayersAvailable())
	{
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}

	if (FAILED(hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		deviceFlags,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&device,
		&d3dFeatureLevel,
		&context
	)))
	{
		return hr;
	}

	SetDebugObjectName(context.Get(), "Device context");

	// Store pointers to the Direct3D 11 API device and immediate context.
	DirectxHelper::ThrowIfFailed(
		device.As(&d3dDevice)
	);

	DirectxHelper::ThrowIfFailed(
		context.As(&d3dContext)
	);

	// Set the viewport
	ResizeScene(640, 480);

	return S_OK;
}

HRESULT Application::LoadDemo()
{
	std::unique_ptr<IGeometryFactory> geometryFactoryPtr = make_unique<D3D11GeometryFactory>(d3dDevice.Get());

	DemoLoader loader(geometryFactoryPtr.get(), d3dDevice.Get());

	mainTimeline = unique_ptr<Timeline>(loader.loadDemoFromFile(std::wstring(L"")));

	return S_OK;
}

void Application::ResizeScene(int width, int height) {
	if (!d3dDevice)
	{
		return;
	}

	// Set up the viewport.
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	ID3D11RenderTargetView* nullViews[] = { nullptr };
	d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	renderTargetView = nullptr;
	depthStencilView = nullptr;
	d3dContext->Flush();

	if (swapChain)
	{
		DirectxHelper::ThrowIfFailed(swapChain->ResizeBuffers(backBufferCount, width, height, DXGI_FORMAT_UNKNOWN, 0));
	}
	else
	{
		UINT numQualityLevels = 1;

		d3dDevice->CheckMultisampleQualityLevels1(DXGI_FORMAT_R8G8B8A8_UNORM, 4, 0, &numQualityLevels);
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc = { 0 };

		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.BufferCount = backBufferCount;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;                               // how many multisamples
		swapChainDesc.SampleDesc.Quality = numQualityLevels - 1;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		//swapChainDesc.Scaling = DXGI_SCALING_NONE;
		//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;

		swapChainFullScreenDesc.Windowed = true;

		// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
		ComPtr<IDXGIDevice3> dxgiDevice;
		DirectxHelper::ThrowIfFailed(
			d3dDevice.As(&dxgiDevice)
		);

		ComPtr<IDXGIAdapter> dxgiAdapter;
		DirectxHelper::ThrowIfFailed(
			dxgiDevice->GetAdapter(&dxgiAdapter)
		);

		ComPtr<IDXGIFactory2> dxgiFactory;
		DirectxHelper::ThrowIfFailed(
			dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory))
		);

		DirectxHelper::ThrowIfFailed(
			dxgiFactory->CreateSwapChainForHwnd(
				d3dDevice.Get(),
				hWnd,
				&swapChainDesc,
				&swapChainFullScreenDesc,
				nullptr,
				&swapChain
			)
		);

		DirectxHelper::ThrowIfFailed(
			dxgiDevice->SetMaximumFrameLatency(1)
		);
	}

	// Get buffer and create a render-target-view.
	ComPtr<ID3D11Texture2D> backBufferTexture;
	ComPtr<ID3D11Texture2D> depthStencilTexture;

	// TODO: handle multiple render targets
	for (UINT i = 0; i < renderTargetCount; i++) {
		DirectxHelper::ThrowIfFailed(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBufferTexture)));

		SetDebugObjectName(backBufferTexture.Get(), "Backbuffer texture");

		DirectxHelper::ThrowIfFailed(d3dDevice->CreateRenderTargetView(backBufferTexture.Get(), nullptr, &renderTargetView));

		SetDebugObjectName(renderTargetView.Get(), "Backbuffer");
	}


	// Create a depth stencil view for use with 3D rendering if needed.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT>(width),
		static_cast<UINT>(height),
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
	);

	DirectxHelper::ThrowIfFailed(
		d3dDevice->CreateTexture2D(
			&depthStencilDesc,
			nullptr,
			&depthStencilTexture
		)
	);

	SetDebugObjectName(depthStencilTexture.Get(), "Depth-stencil texture");

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	DirectxHelper::ThrowIfFailed(
		d3dDevice->CreateDepthStencilView(
			depthStencilTexture.Get(),
			&depthStencilViewDesc,
			&depthStencilView
		)
	);

	SetDebugObjectName(depthStencilView.Get(), "Depth-stencil view");

	// create the blend state.
	D3D11_BLEND_DESC blendDesc = { 0 };
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	DirectxHelper::ThrowIfFailed(
		d3dDevice->CreateBlendState(
			&blendDesc,
			&blendState
		)
	);

	SetDebugObjectName(blendState.Get(), "Blend state");

	// create the sampler
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = d3dFeatureLevel > D3D_FEATURE_LEVEL_9_1 ? 4 : 2;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	// allow use of all mip levels
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	DirectxHelper::ThrowIfFailed(
		d3dDevice->CreateSamplerState(
			&samplerDesc,
			&samplerState
		)
	);

	SetDebugObjectName(samplerState.Get(), "Sampler state");

	d3dContext->RSSetViewports(1, &vp);
}

void Application::RunMessageLoop()
{
	MSG msg;

	bool done = false;

	DWORD startTick = GetTickCount();

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))			// Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)
			{
				done = true;
			}

			// translate keystroke messages into the right format
			TranslateMessage(&msg);

			// send the message to the WindowProc function
			DispatchMessage(&msg);
		}
		else
		{
			Render(GetTickCount() - startTick);
		}
	}
}


void Application::Render(DWORD tickCount)
{
	if (!swapChain || !renderTargetView)
	{
		return;
	}

	ID3D11RenderTargetView *const targets[1] = { renderTargetView.Get() };
	d3dContext->OMSetRenderTargets(1, targets, depthStencilView.Get());

	float clearColor[4] = { 0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f };
	d3dContext->ClearRenderTargetView(renderTargetView.Get(), clearColor);
	d3dContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if (mainTimeline)
	{
		mainTimeline->Render(tickCount, d3dContext.Get(), swapChain.Get());
	}

	swapChain->Present(0, 0);
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

#ifdef _DEBUG
void Application::DumpD3DDebug()
{
	// This is debug code to free up all allocated D3D resources, and generate debug output for leaked objects.
	// The way this works is that D3D keeps track of all objects. When we destroy the m_main and m_deviceResources classes, 
	// all D3D objects should be released. If any objects that D3D thinks are not released are being leaked and the code should be examined.
	
	// Get the debug interface for the device.
	ComPtr<IDXGIDebug1> debugInterface = nullptr;
	/*
	if (FAILED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debugInterface))))
	{
		return;
	}

	// Dump the list of leaked objects to the debugger output window.
	DirectxHelper::ThrowIfFailed(debugInterface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL));
	*/
}
#endif
