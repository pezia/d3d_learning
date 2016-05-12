#pragma once

#include "common.h"

using namespace Microsoft::WRL;
using namespace DirectxHelper;

class D3DResources
{
public:
	ComPtr<ID3D11Device2>           d3dDevice;
	ComPtr<ID3D11DeviceContext2>    d3dContext;
	ComPtr<IDXGIFactory3>           dxgiFactory;
	ComPtr<IDXGISwapChain1>         swapChain;
	ComPtr<ID3D11RenderTargetView>  renderTargetView;
	ComPtr<ID3D11DepthStencilView>  depthStencilView;
	ComPtr<ID3D11BlendState>        blendState;
	ComPtr<ID3D11SamplerState>      samplerState;

	D3D11_VIEWPORT                  viewport;
	D3D_FEATURE_LEVEL               d3dFeatureLevel;
	DXGI_FORMAT                     backBufferFormat;
	DXGI_FORMAT                     depthBufferFormat;

	HWND                            hWnd;

	UINT                            backBufferCount;
	UINT                            renderTargetCount;
	UINT							renderTargetWidth;
	UINT							renderTargetHeight;

public:

	D3DResources(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat) :
		backBufferFormat(backBufferFormat),
		depthBufferFormat(depthBufferFormat),
		viewport(),
		d3dFeatureLevel(),
		d3dDevice(nullptr),
		d3dContext(nullptr),
		swapChain(nullptr),
		renderTargetView(nullptr),
		depthStencilView(nullptr),
		blendState(nullptr),
		samplerState(nullptr),
		hWnd(nullptr),
		backBufferCount(2),
		renderTargetCount(1),
		renderTargetWidth(1),
		renderTargetHeight(1)
	{
		CreateDeviceResources();
	}

	~D3DResources()
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

		blendState = nullptr;
		samplerState = nullptr;
		swapChain = nullptr;
		depthStencilView = nullptr;
		renderTargetView = nullptr;
		d3dContext = nullptr;
		d3dDevice = nullptr;
		hWnd = nullptr;
	}

	void setHwnd(HWND hWnd)
	{
		this->hWnd = hWnd;

		RECT rect;
		GetWindowRect(hWnd, &rect);

		setSize(rect.right - rect.left, rect.bottom - rect.top);
	}

	void setSize(UINT width, UINT height)
	{
		// Set up the viewport.
		viewport.Width = (FLOAT)width;
		viewport.Height = (FLOAT)height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		UpdateRenderTargetSize();
		CreateWindowSizeDependentResources();
	}
private:
	void CreateDeviceResources()
	{
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;

		UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));

		if (SdkLayersAvailable())
		{
			deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		}

		ThrowIfFailed(D3D11CreateDevice(
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
		));

		SetDebugObjectName(context.Get(), "Device context");

		// Store pointers to the Direct3D 11 API device and immediate context.
		ThrowIfFailed(
			device.As(&d3dDevice)
		);

		ThrowIfFailed(
			context.As(&d3dContext)
		);
	}

	void CreateWindowSizeDependentResources()
	{
		if (hWnd == nullptr)
		{
			throw std::exception("Set hWnd first!");
		}

		ID3D11RenderTargetView* nullViews[] = { nullptr };

		d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);

		renderTargetView = nullptr;
		depthStencilView = nullptr;

		d3dContext->Flush();


		if (swapChain != nullptr) {
			ThrowIfFailed(swapChain->ResizeBuffers(backBufferCount, renderTargetWidth, renderTargetHeight, backBufferFormat, 0));
		}
		else
		{
			CreateSwapChain();
		}

		CreateBackBufferView();
		CreateDepthStencilView();
		CreateBlendState();
		CreateSamplerState();

		d3dContext->RSSetViewports(1, &viewport);
	}

	void CreateSwapChain()
	{
		UINT numQualityLevels = 1;

		d3dDevice->CheckMultisampleQualityLevels1(backBufferFormat, 4, 0, &numQualityLevels);
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc = { 0 };

		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.BufferCount = backBufferCount;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
		swapChainDesc.Format = backBufferFormat;
		swapChainDesc.SampleDesc.Count = 1;                               // how many multisamples
		swapChainDesc.SampleDesc.Quality = numQualityLevels - 1;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		//swapChainDesc.Scaling = DXGI_SCALING_NONE;
		//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Width = renderTargetWidth;
		swapChainDesc.Height = renderTargetHeight;

		swapChainFullScreenDesc.Windowed = true;

		// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
		ComPtr<IDXGIDevice3> dxgiDevice;
		ThrowIfFailed(
			d3dDevice.As(&dxgiDevice)
		);

		ComPtr<IDXGIAdapter> dxgiAdapter;
		ThrowIfFailed(
			dxgiDevice->GetAdapter(&dxgiAdapter)
		);

		ComPtr<IDXGIFactory2> dxgiFactory;
		ThrowIfFailed(
			dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory))
		);

		ThrowIfFailed(
			dxgiFactory->CreateSwapChainForHwnd(
				d3dDevice.Get(),
				hWnd,
				&swapChainDesc,
				&swapChainFullScreenDesc,
				nullptr,
				&swapChain
			)
		);

		ThrowIfFailed(
			dxgiDevice->SetMaximumFrameLatency(1)
		);
	}

	void CreateBackBufferView()
	{
		// Get buffer and create a render-target-view.
		ComPtr<ID3D11Texture2D> backBufferTexture;

		// TODO: handle multiple render targets
		for (UINT i = 0; i < renderTargetCount; i++) {
			ThrowIfFailed(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBufferTexture)));

			SetDebugObjectName(backBufferTexture.Get(), "Backbuffer texture");

			ThrowIfFailed(d3dDevice->CreateRenderTargetView(backBufferTexture.Get(), nullptr, &renderTargetView));

			SetDebugObjectName(renderTargetView.Get(), "Backbuffer");
		}
	}

	void CreateDepthStencilView()
	{
		ComPtr<ID3D11Texture2D> depthStencilTexture;

		// Create a depth stencil view for use with 3D rendering if needed.
		CD3D11_TEXTURE2D_DESC depthStencilDesc(
			depthBufferFormat,
			renderTargetWidth,
			renderTargetHeight,
			1, // This depth stencil view has only one texture.
			1, // Use a single mipmap level.
			D3D11_BIND_DEPTH_STENCIL
		);

		ThrowIfFailed(
			d3dDevice->CreateTexture2D(
				&depthStencilDesc,
				nullptr,
				&depthStencilTexture
			)
		);

		SetDebugObjectName(depthStencilTexture.Get(), "Depth-stencil texture");

		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
		ThrowIfFailed(
			d3dDevice->CreateDepthStencilView(
				depthStencilTexture.Get(),
				&depthStencilViewDesc,
				&depthStencilView
			)
		);

		SetDebugObjectName(depthStencilView.Get(), "Depth-stencil view");
	}

	void CreateBlendState()
	{
		D3D11_BLEND_DESC blendDesc = { 0 };

		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		ThrowIfFailed(
			d3dDevice->CreateBlendState(
				&blendDesc,
				&blendState
			)
		);

		SetDebugObjectName(blendState.Get(), "Blend state");

	}

	void CreateSamplerState()
	{
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

		ThrowIfFailed(
			d3dDevice->CreateSamplerState(
				&samplerDesc,
				&samplerState
			)
		);

		SetDebugObjectName(samplerState.Get(), "Sampler state");
	}

	void UpdateRenderTargetSize()
	{
		// Prevent zero size DirectX content from being created.
		renderTargetWidth = (UINT)max(viewport.Width, 1);
		renderTargetHeight = (UINT)max(viewport.Height, 1);
	}
};
