#pragma once

#include "common.h"
#include "classes.h"

#include <D3Dcompiler.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

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
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile("assets\\scene1.blend",
			aiProcess_OptimizeMeshes
			| aiProcess_CalcTangentSpace
			| aiProcess_Triangulate
//			| aiProcess_JoinIdenticalVertices
//			| aiProcess_SortByPType
			| aiProcess_FlipWindingOrder
		);

		HRESULT hr;
		ComPtr<ID3DBlob> VS, PS, HS, DS, errors;

		std::unique_ptr<Timeline> timeline(new Timeline());

		RenderTimelineEvent* renderEvent = new RenderTimelineEvent();
		timeline->timelineEvents.push_back(renderEvent);

		renderEvent->camera.position = XMVectorSet(0.0f, 2.0f, 4.0f, 0.0f);
		renderEvent->camera.target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		renderEvent->camera.up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		auto light1 = OmniLight();
		light1.range = 100.0f;
		light1.diffuseColor =  XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
		light1.position = XMFLOAT4(0.0f, 10.0f, 0.0f, 0.0f);

		auto light2 = OmniLight();
		light2.range = 100.0f;
		light2.diffuseColor = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);
		light2.position = XMFLOAT4(0.0f, -10.0f, 0.0f, 0.0f);

		auto light3 = OmniLight();
		light3.range = 100.0f;
		light3.diffuseColor = XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f);
		light3.position = XMFLOAT4(0.0f, 0, 10.0f, 0.0f);

		auto light4 = OmniLight();
		light4.range = 100.0f;
		light4.diffuseColor = XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f);
		light4.position = XMFLOAT4(0.0f, 0.0f, -10.0f, 0.0f);

		renderEvent->omniLights.push_back(light1);
		renderEvent->omniLights.push_back(light2);
		renderEvent->omniLights.push_back(light3);
		renderEvent->omniLights.push_back(light4);

		if (FAILED(hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "VShader", "vs_4_0", D3DCOMPILE_DEBUG, 0, &VS, &errors))) {
			char *verror = (char *)errors->GetBufferPointer();
			MessageBox(NULL, verror, "Error compiling the vertex shader", MB_ICONASTERISK);
			DirectxHelper::ThrowIfFailed(hr);
		}

		if (FAILED(hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "PShader", "ps_4_0", D3DCOMPILE_DEBUG, 0, &PS, &errors))) {
			char *verror = (char *)errors->GetBufferPointer();
			MessageBox(NULL, verror, "Error compiling the vertex shader", MB_ICONASTERISK);
			DirectxHelper::ThrowIfFailed(hr);
		}
		/*
		if (FAILED(hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "SubDToBezierHS", "ps_4_0", D3DCOMPILE_DEBUG, 0, &HS, &errors))) {
			char *verror = (char *)errors->GetBufferPointer();
			MessageBox(NULL, verror, "Error compiling the hull shader", MB_ICONASTERISK);
			DirectxHelper::ThrowIfFailed(hr);
		}
		/*
		if (FAILED(hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "DomainShader", "ps_4_0", D3DCOMPILE_DEBUG, 0, &DS, &errors))) {
			char *verror = (char *)errors->GetBufferPointer();
			MessageBox(NULL, verror, "Error compiling the hull shader", MB_ICONASTERISK);
			DirectxHelper::ThrowIfFailed(hr);
		}
		*/

		// create the input layout object
		D3D11_INPUT_ELEMENT_DESC inputElementDescriptor[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};


		for (UINT i = 0; i<scene->mNumMeshes; i++)
		{
			auto importedMesh = scene->mMeshes[i];
			Mesh* mesh = geometryFactory->createFromAiMesh(scene->mMeshes[i]);

			DirectxHelper::ThrowIfFailed(d3dDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &mesh->vertexShader));
			DirectxHelper::ThrowIfFailed(d3dDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &mesh->pixelShader));
			//DirectxHelper::ThrowIfFailed(d3dDevice->CreateHullShader(HS->GetBufferPointer(), HS->GetBufferSize(), NULL, &mesh->hullShader));
			//DirectxHelper::ThrowIfFailed(d3dDevice->CreateDomainShader(DS->GetBufferPointer(), DS->GetBufferSize(), NULL, &mesh->domainShader));

			DirectxHelper::ThrowIfFailed(d3dDevice->CreateInputLayout(inputElementDescriptor, ARRAYSIZE(inputElementDescriptor), VS->GetBufferPointer(), VS->GetBufferSize(), &mesh->vertexLayout));

			SetDebugObjectName(mesh->vertexLayout.Get(), "Vertex layout");

			SetDebugObjectName(mesh->vertexShader.Get(), "Vertex shader");
			SetDebugObjectName(mesh->pixelShader.Get(), "Pixel shader");

			mesh->modelMatrix = XMMatrixRotationX(-XM_PIDIV2);

			renderEvent->meshes.push_back(mesh);
		}

		return timeline.release();
	}
};
