#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	if (vertices) {
		delete[] vertices;
	}

	if (indices) {
		delete[] indices;
	}

	if (vertexBuffer) {
		vertexBuffer->Release();
	}

	if (indexBuffer) {
		indexBuffer->Release();
	}
}
