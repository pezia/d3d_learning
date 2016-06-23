#pragma once

#include "common.h"

using namespace Microsoft::WRL;

class Mesh
{
public:
	void * operator new(size_t nSize)
	{
		return _aligned_malloc(nSize, 16);
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}

	Mesh() noexcept :
		vertexBuffer(nullptr), indexBuffer(nullptr), vertexLayout(nullptr), vertexShader(nullptr), pixelShader(nullptr), primitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{
		modelMatrix = XMMatrixIdentity();
	}

	~Mesh()
	{
		hullShader   = nullptr;
		domainShader = nullptr;
		vertexShader = nullptr;
		pixelShader  = nullptr;
		vertexBuffer = nullptr;
		indexBuffer  = nullptr;
		vertexLayout = nullptr;
	}

	std::wstring name;

	D3D_PRIMITIVE_TOPOLOGY primitiveTopology;

	DWORD numVertices;
	DWORD numIndices;

	ComPtr<ID3D11Buffer>       vertexBuffer;
	ComPtr<ID3D11Buffer>       indexBuffer;

	ComPtr<ID3D11InputLayout>  vertexLayout;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader>  pixelShader;
	ComPtr<ID3D11HullShader>   hullShader;
	ComPtr<ID3D11DomainShader> domainShader;

	XMMATRIX modelMatrix;
};
