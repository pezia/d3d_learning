#pragma once

#include "common.h"
#include "classes.h"

class DemoLoader
{
private:
	IGeometryFactory* geometryFactory;
	ID3D11Device2* d3dDevice;

public:
	DemoLoader(IGeometryFactory* geometryFactory, ID3D11Device2* d3dDevice) :
		geometryFactory(geometryFactory),
		d3dDevice(d3dDevice)
	{
	}

	virtual ~DemoLoader()
	{
	}

	Timeline* loadDemoFromFile(std::wstring filePath)
	{
		HRESULT hr;
		ComPtr<ID3DBlob> VS, PS, errors;

		std::unique_ptr<Timeline> timeline(new Timeline());

		RenderTimelineEvent* renderEvent = new RenderTimelineEvent();
		timeline->timelineEvents.push_back(renderEvent);

		renderEvent->camera.position = XMVectorSet(0.0f, 2.0f, 4.0f, 0.0f);
		renderEvent->camera.target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		renderEvent->camera.up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		OmniLight* light1 = new OmniLight();
		light1->range = 100.0f;
		light1->diffuseColor = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
		light1->position = XMFLOAT4(0.0f, 10.0f, 0.0f, 0.0f);

		OmniLight* light2 = new OmniLight();
		light2->range = 100.0f;
		light2->diffuseColor = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);
		light2->position = XMFLOAT4(0.0f, -10.0f, 0.0f, 0.0f);


		OmniLight* light3 = new OmniLight();
		light3->range = 100.0f;
		light3->diffuseColor = XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f);
		light3->position = XMFLOAT4(0.0f, 0, 10.0f, 0.0f);


		OmniLight* light4 = new OmniLight();
		light4->range = 100.0f;
		light4->diffuseColor = XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f);
		light4->position = XMFLOAT4(0.0f, 0.0f, -10.0f, 0.0f);

		renderEvent->omniLights.push_back(light1);
		renderEvent->omniLights.push_back(light2);
		renderEvent->omniLights.push_back(light3);
		renderEvent->omniLights.push_back(light4);

		Mesh* mesh1 = geometryFactory->createReferenceAxis();

		if (FAILED(hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &errors, 0))) {
			char *verror = (char *)errors->GetBufferPointer();
			MessageBox(NULL, verror, "Error compiling the vertex shader", MB_ICONASTERISK);
			DirectxHelper::ThrowIfFailed(hr);
		}

		if (FAILED(hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &errors, 0))) {
			char *verror = (char *)errors->GetBufferPointer();
			MessageBox(NULL, verror, "Error compiling the vertex shader", MB_ICONASTERISK);
			DirectxHelper::ThrowIfFailed(hr);
		}

		// encapsulate both shaders into shader objects
		DirectxHelper::ThrowIfFailed(d3dDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &mesh1->vertexShader));
		DirectxHelper::ThrowIfFailed(d3dDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &mesh1->pixelShader));

		SetDebugObjectName(mesh1->vertexShader.Get(), "Vertex shader");
		SetDebugObjectName(mesh1->pixelShader.Get(), "Pixel shader");

		// create the input layout object
		D3D11_INPUT_ELEMENT_DESC inputElementDescriptor[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DirectxHelper::ThrowIfFailed(d3dDevice->CreateInputLayout(inputElementDescriptor, ARRAYSIZE(inputElementDescriptor), VS->GetBufferPointer(), VS->GetBufferSize(), &mesh1->vertexLayout));

		SetDebugObjectName(mesh1->vertexLayout.Get(), "Vertex layout");

		renderEvent->meshes.push_back(mesh1);

		return timeline.release();
	}
};
