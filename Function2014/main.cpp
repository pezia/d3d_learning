// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <xnamath.h>
#include "types.h"
#include "Mesh.h"

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) \
	if(x != NULL)\
				{\
		x->Release();\
		x = NULL;\
				}
#endif

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
XMMATRIX g_World;
XMMATRIX g_View;
XMMATRIX g_Projection;

Mesh *cube;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
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
	wc.lpszClassName = "OpenGL";					// Set The Class Name

	if (!RegisterClass(&wc))						// Attempt To Register The Window Class
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;							// Exit And Return FALSE
	}

	// create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL,
		"OpenGL",    // name of the window class
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


bool InitD3D(HWND hWnd) {
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hWnd;                                // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching


	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		NULL,
		&devcon);

	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
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
	ID3D10Blob *VS, *PS, *errors;

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
	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	// set the shader objects
	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	dev->CreateInputLayout(ied, 3, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);

	return true;
}

bool InitGraphics()
{
	cube = new Mesh();

	cube->vertexCount = 24;
	cube->vertices = new SimpleVertex[24] {
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
	};

	cube->indexCount = 36;
	cube->indices = new WORD[36] {
		3, 1, 0,
			2, 1, 3,

			6, 4, 5,
			7, 4, 6,

			11, 9, 8,
			10, 9, 11,

			14, 12, 13,
			15, 12, 14,

			19, 17, 16,
			18, 17, 19,

			22, 20, 21,
			23, 20, 22
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(SimpleVertex) * cube->vertexCount;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer


	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = cube->vertices;

	dev->CreateBuffer(&bd, &InitData, &cube->vertexBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * cube->indexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = cube->indices;

	dev->CreateBuffer(&bd, &InitData, &cube->indexBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = 432;// sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dev->CreateBuffer(&bd, NULL, &pConstantBuffer);

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

	if (cube) {
		delete cube;
		cube = NULL;
	}
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

		if (FAILED(dev->CreateRenderTargetView(pBuffer, NULL,
			&backbuffer))) {
			// Perform error handling here!
		}

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

	XMVECTOR eye = XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);
	XMVECTOR at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	g_View = XMMatrixLookAtLH(eye, at, up);
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (float)height, 0.01f, 100.0f);
	g_World = XMMatrixMultiply(
		XMMatrixRotationY(XM_PIDIV4 * sin(time / 10.0f)),
		XMMatrixRotationX(XM_PIDIV4 * sin(time / 13.0f)));

	XMFLOAT3 vLightDirs[2] =
	{
		XMFLOAT3(0.1f, 0.2f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, -1.0f),
	};

	XMFLOAT4 vLightColors[2] =
	{
		XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f),
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)
	};

	// Rotate the second light around the origin
	XMMATRIX mRotate = XMMatrixRotationY(time);
	XMVECTOR vLightDir = XMLoadFloat3(&vLightDirs[1]);
	vLightDir = XMVector3Transform(vLightDir, mRotate);
	XMStoreFloat3(&vLightDirs[1], vLightDir);

	cb.mWorld = XMMatrixTranspose(g_World);
	cb.mView = XMMatrixTranspose(g_View);
	cb.mProjection = XMMatrixTranspose(g_Projection);

	XMStoreFloat3(&cb.vEyePosition, eye);

	cb.vLightDir[0] = vLightDirs[0];
	cb.vLightDir[1] = vLightDirs[1];
	cb.vLightColor[0] = vLightColors[0];
	cb.vLightColor[1] = vLightColors[1];
	cb.fLightIntensity[0] = 0.3f;
	cb.fLightIntensity[1] = 0.4f;
	cb.vAmbientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cb.fAmbientIntensity = 0.5f;

	D3D11_MAPPED_SUBRESOURCE sr;
	devcon->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sr);
	memcpy(sr.pData, &cb, sizeof(ConstantBuffer));
	devcon->Unmap(pConstantBuffer, 0);

	// select which vertex and index buffer to display
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &cube->vertexBuffer, &stride, &offset);
	devcon->IASetIndexBuffer(cube->indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// select which primtive type we are using
	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->VSSetShader(pVS, NULL, 0);
	devcon->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	devcon->PSSetShader(pPS, NULL, 0);
	devcon->PSSetConstantBuffers(0, 1, &pConstantBuffer);

	devcon->DrawIndexed(36, 0, 0);

	// switch the back buffer and the front buffer
	swapchain->Present(0, 0);
}
