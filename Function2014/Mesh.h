#pragma once

#include "common.h"

using namespace Microsoft::WRL;

class Mesh
{
public:
	Mesh();
	~Mesh();

	std::wstring name;

	D3D_PRIMITIVE_TOPOLOGY primitiveTopology;

	DWORD numVertices;
	DWORD numIndices;

	ComPtr<ID3D11Buffer>       vertexBuffer;
	ComPtr<ID3D11Buffer>       indexBuffer;

	ComPtr<ID3D11InputLayout>  vertexLayout;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader>  pixelShader;

	XMMATRIX modelMatrix;
};
