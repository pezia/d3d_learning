#include "Mesh.h"

Mesh::Mesh() :
	vertexBuffer(nullptr), indexBuffer(nullptr), vertexLayout(nullptr), vertexShader(nullptr), pixelShader(nullptr), primitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
	modelMatrix = XMMatrixIdentity();
}

Mesh::~Mesh()
{
	vertexShader = nullptr;
	pixelShader = nullptr;
	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	vertexLayout = nullptr;
}
