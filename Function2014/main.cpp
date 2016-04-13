// include the basic windows header file
#include <iostream>
#include <math.h>
#include <vector>
#include <D3DX11.h>
#include <xnamath.h>
#include "debug.h"
#include "types.h"
#include "macros.h"
#include "Mesh.h"
#include "IGeometryFactory.h"
#include "D3D11GeometryFactory.h"
#include "DirectxHelper.h"

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "dxgi.lib")

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

// function prototypes
bool InitD3D(HWND hWnd);
bool InitGraphics();
void CleanD3D();
bool InitPipeline();

void ResizeScene(int width, int height);
void Render(DWORD tickCount);

HWND hWnd;
HINSTANCE hInstance;

bool fullscreen = false;
int width = 640;
int height = 480;
DWORD startTime;

DEVMODE dmScreenSettings;

DXGI_SWAP_CHAIN_DESC scd;
IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView *backbuffer;

ID3D11VertexShader *pVS;    // the vertex shader
ID3D11PixelShader *pPS;     // the pixel shader

ID3D11Buffer *pConstantBuffer;

ID3D11InputLayout *pLayout;

ConstantBuffer cb;
XMMATRIX g_Model;
XMMATRIX g_View;
XMMATRIX g_Projection;

IGeometryFactory* geometryFactory;

std::vector<Mesh *> sceneObjects = std::vector<Mesh *>();

int WINAPI WinMain(
	_In_ HINSTANCE hInst, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	hInstance = hInst;

	if (fullscreen)
	{
		dmScreenSettings.dmSize = sizeof(DEVMODE);
		dmScreenSettings.dmPelsWidth = width;
		dmScreenSettings.dmPelsHeight = height;
		dmScreenSettings.dmBitsPerPel = 24;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			fullscreen = false;
		}
	}

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// Redraw On Move, And Own DC For Window
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.cbClsExtra = 0;						// No Extra Window Data
	wc.cbWndExtra = 0;						// No Extra Window Data
	wc.hInstance = hInstance;					// Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = NULL;						// No Background Required For GL
	wc.lpszMenuName = NULL;						// We Don't Want A Menu
	wc.lpszClassName = "Intro";					// Set The Class Name

	if (!RegisterClass(&wc))						// Attempt To Register The Window Class
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;							// Exit And Return FALSE
	}

	// create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL,
		"Intro",    // name of the window class
		"",   // title of the window
		WS_OVERLAPPEDWINDOW,    // window style
		0,    // x-position of the window
		0,    // y-position of the window
		width,    // width of the window
		height,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		hInstance,    // application handle
		NULL);    // used with multiple windows, NULL

	if (!InitD3D(hWnd)) {
		MessageBox(hWnd, "InitD3D failed", "Failure", MB_ICONASTERISK);
		CleanD3D();
		return -1;
	}

	if (!InitPipeline()) {
		MessageBox(hWnd, "InitPipeline failed", "Failure", MB_ICONASTERISK);
		CleanD3D();
		return -1;
	}

	if (!InitGraphics()) {
		MessageBox(hWnd, "InitGraphics failed", "Failure", MB_ICONASTERISK);
		CleanD3D();
		return -1;
	}

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);								// Sets Keyboard Focus To The Window

	MSG msg;

	bool done = false;

	startTime = GetTickCount();

	while (!done)								// Loop That Runs Until done=TRUE
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))			// Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)
			{
				done = true;
			}
			else
			{
				// translate keystroke messages into the right format
				TranslateMessage(&msg);

				// send the message to the WindowProc function
				DispatchMessage(&msg);
			}
		}
		else
		{
			Render(GetTickCount() - startTime);
		}
	}

	CleanD3D();

	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}

IDXGIAdapter1* selectAdapter()
{
	int i = 0;
	IDXGIAdapter1 * pAdapter;
	IDXGIFactory1 * pFactory;
	DXGI_ADAPTER_DESC1 adapterDesc;
	std::vector <IDXGIAdapter1*> adapters;

	DirectxHelper::ThrowIfFailed(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pFactory)));

	while (pFactory->EnumAdapters1(i++, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		adapters.push_back(pAdapter);
	}

	// TODO: show dialog with the adapters
	for each (IDXGIAdapter1* adapter in adapters)
	{
		adapter->GetDesc1(&adapterDesc);
	}

	SAFE_RELEASE(pFactory);
	// TODO: release unused adapters

	return adapters.at(0);
}

bool InitD3D(HWND hWnd) {
	D3D_FEATURE_LEVEL featureLevel;

	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hWnd;                                // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching

	IDXGIAdapter1 * pAdapter;

	pAdapter = selectAdapter();

	DirectxHelper::ThrowIfFailed(D3D11CreateDeviceAndSwapChain(pAdapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		&featureLevel,
		&devcon));

	SetDebugObjectName(devcon, "Device context");

	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	SetDebugObjectName(pBackBuffer, "Backbuffer texture");

	// use the back buffer address to create the render target
	DirectxHelper::ThrowIfFailed(dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer));
	SetDebugObjectName(backbuffer, "Backbuffer");

	pBackBuffer->Release();

	// set the render target as the back buffer
	devcon->OMSetRenderTargets(1, &backbuffer, NULL);

	// Set the viewport
	ResizeScene(width, height);

	return true;
}


bool InitPipeline()
{
	// load and compile the two shaders
	ID3DBlob *VS, *PS, *errors;

	if (FAILED(D3DX11CompileFromFile("shaders.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &errors, 0))) {
		char *verror = (char *)errors->GetBufferPointer();
		MessageBox(hWnd, verror, "Error compiling the vertex shader", MB_ICONASTERISK);
		SAFE_RELEASE(errors);
		return false;
	}

	if (FAILED(D3DX11CompileFromFile("shaders.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &errors, 0))) {
		char *verror = (char *)errors->GetBufferPointer();
		MessageBox(hWnd, verror, "Error compiling the vertex shader", MB_ICONASTERISK);
		SAFE_RELEASE(errors);
		return false;
	}

	// encapsulate both shaders into shader objects
	DirectxHelper::ThrowIfFailed(dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS));
	DirectxHelper::ThrowIfFailed(dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS));

	SetDebugObjectName(pVS, "Vertex shader");
	SetDebugObjectName(pPS, "Pixel shader");

	// set the shader objects
	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC inputElementDescriptor[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	DirectxHelper::ThrowIfFailed(dev->CreateInputLayout(inputElementDescriptor, ARRAYSIZE(inputElementDescriptor), VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout));
	SetDebugObjectName(pLayout, "Input layout");
	devcon->IASetInputLayout(pLayout);

	return true;
}

bool InitGraphics()
{
	geometryFactory = new D3D11GeometryFactory(dev);
	//Mesh* cube = geometryFactory->createCube();
	Mesh* referenceAxis = geometryFactory->createReferenceAxis();
	//Mesh* sphere = geometryFactory->createSphere(128);
	//sceneObjects.push_back(cube);
	sceneObjects.push_back(referenceAxis);
	//sceneObjects.push_back(sphere);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	DirectxHelper::ThrowIfFailed(dev->CreateBuffer(&bd, NULL, &pConstantBuffer));

	SetDebugObjectName(pConstantBuffer, "Constant buffer");

	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

void CleanD3D()
{
	swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

	// close and release all existing COM objects
	SAFE_RELEASE(pVS);
	SAFE_RELEASE(pPS);

	SAFE_RELEASE(swapchain);
	SAFE_RELEASE(backbuffer);
	SAFE_RELEASE(devcon);
	SAFE_RELEASE(dev);

	SAFE_RELEASE(pConstantBuffer);
	SAFE_RELEASE(pLayout);

	for each (Mesh* mesh in sceneObjects)
	{
		SAFE_DELETE(mesh);
	}

	SAFE_DELETE(geometryFactory);
}

void ResizeScene(int newWidth, int newHeight) {
	width = newWidth;
	height = newHeight;

	if (swapchain)
	{
		devcon->OMSetRenderTargets(0, 0, 0);

		// Release all outstanding references to the swap chain's buffers.
		backbuffer->Release();

		// Preserve the existing buffer count and format.
		// Automatically choose the width and height to match the client rect for HWNDs.
		if (FAILED(swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0))) {
			// Perform error handling here!
		}

		// Get buffer and create a render-target-view.
		ID3D11Texture2D* pBuffer;
		if (FAILED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
			(void**)&pBuffer))) {
			// Perform error handling here!
		}

		SetDebugObjectName(pBuffer, "Backbuffer texture");

		if (FAILED(dev->CreateRenderTargetView(pBuffer, NULL,
			&backbuffer))) {
			// Perform error handling here!
		}

		SetDebugObjectName(backbuffer, "Backbuffer");

		pBuffer->Release();

		devcon->OMSetRenderTargets(1, &backbuffer, NULL);

		// Set up the viewport.
		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)width;
		vp.Height = (FLOAT)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		devcon->RSSetViewports(1, &vp);
	}
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return 0;
		}
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:
		ResizeScene(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}


void Render(DWORD tickCount)
{
	float time = tickCount / 1000.0f;

	float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	devcon->ClearRenderTargetView(backbuffer, clearColor);

	ConstantBuffer cb;

	XMVECTOR eye = XMVectorSet(sin(time), -1.0f, cos(time), 0.0f);
	XMVECTOR at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	g_View = XMMatrixLookAtLH(eye, at, up);
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, (float)width / (float)height, 0.01f, 100.0f);
	g_Model = XMMatrixIdentity();
	/*XMMatrixMultiply(
		XMMatrixRotationY(XM_2PI * sin(time / 3.0f)),
		XMMatrixRotationX(XM_2PI * cos(time / 3.0f)));
		*/
	XMFLOAT4 vLightDirs[2] =
	{
		XMFLOAT4(1.0f, 0.2f, 1.0f, 0.0f),
		XMFLOAT4(1.0f, -1.0f, 0.0f, 0.0f),
	};

	XMFLOAT4 vLightColors[2] =
	{
		XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f),
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)
	};

	// Rotate the second light around the origin
	XMMATRIX mRotate = XMMatrixRotationZ(time * 2);
	XMVECTOR vLightDir = XMLoadFloat4(&vLightDirs[1]);
	vLightDir = XMVector3Transform(vLightDir, mRotate);
	XMStoreFloat4(&vLightDirs[1], vLightDir);

	cb.mModel = XMMatrixTranspose(g_Model);
	cb.mView = XMMatrixTranspose(g_View);
	cb.mProjection = XMMatrixTranspose(g_Projection);

	XMStoreFloat3(&cb.vEyePosition, eye);

	cb.iLightCount = 2;
	cb.vLightDir[0] = vLightDirs[0];
	cb.vLightDir[1] = vLightDirs[1];
	cb.vLightColor[0] = vLightColors[0];
	cb.vLightColor[1] = vLightColors[1];
	cb.fLightIntensity[0] = 0.7f;
	cb.fLightIntensity[1] = 0.7f;
	cb.vAmbientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cb.fAmbientIntensity = 0.05f;

	D3D11_MAPPED_SUBRESOURCE sr;
	devcon->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sr);
	memcpy(sr.pData, &cb, sizeof(ConstantBuffer));
	devcon->Unmap(pConstantBuffer, 0);

	// select which vertex and index buffer to display
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	for each (Mesh* mesh in sceneObjects)
	{
		devcon->IASetVertexBuffers(0, 1, &mesh->vertexBuffer, &stride, &offset);
		devcon->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// select which primtive type we are using
		devcon->IASetPrimitiveTopology(mesh->primitiveTopology);
		devcon->VSSetShader(pVS, NULL, 0);
		devcon->VSSetConstantBuffers(0, 1, &pConstantBuffer);
		devcon->PSSetShader(pPS, NULL, 0);
		devcon->PSSetConstantBuffers(0, 1, &pConstantBuffer);

		devcon->DrawIndexed(mesh->numIndices, 0, 0);
	}

	// switch the back buffer and the front buffer
	swapchain->Present(0, 0);
}
