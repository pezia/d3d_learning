#pragma once

#include "common.h"

class RenderTimelineEvent : public TimelineEventAbstract
{

public:
	std::vector<Mesh*> meshes;
	std::vector<OmniLight*> omniLights;
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
		for each (OmniLight* omniLight in omniLights)
		{
			SAFE_DELETE(omniLight);
		}

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

	void Render(DWORD tick, ID3D11DeviceContext2 *deviceContext, IDXGISwapChain1 *swapChain)
	{
		FLOAT time = tick / 1000.0f;

		SecureZeroMemory(&constantBufferData, sizeof(constantBufferData));
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

		swapChain->GetDesc1(&swapChainDesc);
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;

		if (constantBuffer == nullptr) {
			D3D11_BUFFER_DESC bd = { 0 };
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = ((sizeof(constantBufferData) + 15) / 16) * 16; // multiple of 16 bytes
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;

			ComPtr<ID3D11Device> device;

			deviceContext->GetDevice(&device);
			DirectxHelper::ThrowIfFailed(device->CreateBuffer(&bd, nullptr, &constantBuffer));
			SetDebugObjectName(constantBuffer.Get(), "Constant buffer");
		}

		// TODO move camera position logic to the camera
		camera.position = XMVectorSet(2.0f * sin(time), sin(time), 2.0f * cos(time), 1.0f);
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

		XMMATRIX rotationMatrix = XMMatrixRotationZ(time * XM_2PI);

		for each (OmniLight* omniLight in omniLights)
		{
			constantBufferData.fOmniLightRange[i] = omniLight->range;
			
			constantBufferData.vOmniLightPosition[i] = XMVector4Transform(XMLoadFloat4(&omniLight->position), rotationMatrix);	
			constantBufferData.vOmniLightColor[i] = XMLoadFloat4(&omniLight->diffuseColor);

			i++;
		}

		for each (Mesh* mesh in meshes)
		{
			constantBufferData.mModel = XMMatrixTranspose(mesh->modelMatrix);

			deviceContext->BeginEventInt((std::wstring(L"Render ") + mesh->name).c_str(), 0);

			deviceContext->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &constantBufferData, 0, 0);

			deviceContext->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
			deviceContext->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

			deviceContext->IASetInputLayout(mesh->vertexLayout.Get());
			deviceContext->IASetPrimitiveTopology(mesh->primitiveTopology);
			
			deviceContext->VSSetShader(mesh->vertexShader.Get(), NULL, 0);
			deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			deviceContext->PSSetShader(mesh->pixelShader.Get(), NULL, 0);
			deviceContext->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			deviceContext->DrawIndexed(mesh->numIndices, 0, 0);

			deviceContext->EndEvent();
		}
	}
};
