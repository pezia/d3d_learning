#pragma once

#include "common.h"

class RenderTimelineEvent : public TimelineEventAbstract
{

public:
	std::vector<Mesh*> meshes;
	std::vector<OmniLight> omniLights;
	Camera camera;

protected:
	ComPtr<ID3D11Buffer> constantBuffer;

private:
	ConstantBuffer_t constantBufferData;

public:
	void * operator new(size_t nSize)
	{
		return _aligned_malloc(nSize, 16);
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}

	RenderTimelineEvent()
	{

	}

	~RenderTimelineEvent()
	{
		omniLights.clear();

		for each (Mesh* mesh in meshes)
		{
			SAFE_DELETE(mesh);
		}

		constantBuffer = nullptr;
	}

	std::vector<IProperty> getProperties()
	{
		return std::vector<IProperty>();
	}

	void Render(DWORD tick, const D3DResources* d3dResources)
	{
		FLOAT time = tick / 1000.0f;

		SecureZeroMemory(&constantBufferData, sizeof(constantBufferData));
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

		d3dResources->swapChain->GetDesc1(&swapChainDesc);
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;

		if (constantBuffer == nullptr) {
			D3D11_BUFFER_DESC bd = { 0 };
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = ((sizeof(constantBufferData) + 15) / 16) * 16; // multiple of 16 bytes
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;

			DirectxHelper::ThrowIfFailed(d3dResources->d3dDevice->CreateBuffer(&bd, nullptr, &constantBuffer));
			SetDebugObjectName(constantBuffer.Get(), "Constant buffer");
		}

		// TODO move camera position logic to the camera
		camera.position = XMVectorSet(2.0f * XMScalarSinEst(time), 2.0f * XMScalarSinEst(time), 2.0f * XMScalarCosEst(time), 1.0f);
		camera.up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
		camera.target = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		if (swapChainDesc.Height > 0) {
			camera.aspectRatio = (FLOAT)swapChainDesc.Width / (FLOAT)swapChainDesc.Height;
		}

		XMMATRIX viewMatrix = camera.getViewMatrix();
		XMMATRIX projectionMatrix = camera.getProjectionMatrix();

		constantBufferData.mView = XMMatrixTranspose(viewMatrix);
		constantBufferData.mProjection = XMMatrixTranspose(projectionMatrix);
		constantBufferData.vEyePosition = camera.position;

		constantBufferData.iOmniLightCount = (INT32)omniLights.size();

		int i = 0;

		//XMMATRIX rotationMatrix = XMMatrixRotationZ(time);
		XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(XMVectorSet(sin(time), 0.0f, cos(time), 0.0f));

		for (auto omniLight : omniLights)
		{
			constantBufferData.fOmniLightRange[i] = omniLight.range;
			
			constantBufferData.vOmniLightPosition[i] = XMLoadFloat4(&omniLight.position);
			constantBufferData.vOmniLightColor[i] = XMLoadFloat4(&omniLight.diffuseColor);

			i++;
		}

		for (auto mesh : meshes)
		{
			constantBufferData.mModel                 = XMMatrixTranspose(mesh->modelMatrix);
			constantBufferData.mModelInverseTranspose = XMMatrixTranspose(XMMatrixInverse(nullptr, constantBufferData.mModel));

			d3dResources->d3dContext->BeginEventInt((std::wstring(L"Render ") + mesh->name).c_str(), 0);

			d3dResources->d3dContext->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &constantBufferData, 0, 0);

			d3dResources->d3dContext->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
			d3dResources->d3dContext->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

			d3dResources->d3dContext->IASetInputLayout(mesh->vertexLayout.Get());
			d3dResources->d3dContext->IASetPrimitiveTopology(mesh->primitiveTopology);
			
			d3dResources->d3dContext->VSSetShader(mesh->vertexShader.Get(), NULL, 0);
			d3dResources->d3dContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			d3dResources->d3dContext->PSSetShader(mesh->pixelShader.Get(), NULL, 0);
			d3dResources->d3dContext->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			d3dResources->d3dContext->HSSetShader(mesh->hullShader.Get(), NULL, 0);
			d3dResources->d3dContext->DSSetShader(mesh->domainShader.Get(), NULL, 0);

			d3dResources->d3dContext->DrawIndexed(mesh->numIndices, 0, 0);

			d3dResources->d3dContext->EndEvent();
		}
	}
};
