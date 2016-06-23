#include "D3D11GeometryFactory.h"

#include <locale>
#include <codecvt>
#include <string>

D3D11GeometryFactory::D3D11GeometryFactory(ID3D11Device1* device) : dev(device)
{
}

D3D11GeometryFactory::~D3D11GeometryFactory()
{
	dev = nullptr;
}

Mesh* D3D11GeometryFactory::createCube() {
	Mesh* cube = new Mesh();

	cube->name = L"Cube";
	cube->primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	cube->numVertices = 24;

	std::unique_ptr<SimpleVertex[]> cubeVertices(new SimpleVertex[cube->numVertices] {
		{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // +Y (top face)
		{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // -Y (bottom face)
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // +X (right face)
		{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // -X (left face)
		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) }, // +Z (front face)
		{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }, // -Z (back face)
		{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
	});

	cube->numIndices = 36;
	std::unique_ptr<WORD[]> cubeIndices(new WORD[cube->numIndices] {
			0, 1, 2,
			0, 2, 3,

			4, 5, 6,
			4, 6, 7,

			8, 9, 10,
			8, 10, 11,

			12, 13, 14,
			12, 14, 15,

			16, 17, 18,
			16, 18, 19,

			20, 21, 22,
			20, 22, 23
	});

	this->createBuffers(cube, cubeVertices.get(), cube->numVertices, cubeIndices.get(), cube->numIndices);

	return cube;
}

Mesh* D3D11GeometryFactory::createSphere(const int numSegments) {
	Mesh* sphere = new Mesh();

	sphere->name = L"Sphere";
	sphere->primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	const int numSlices = numSegments / 2;
	const int numVertices = (numSlices + 1) * (numSegments + 1);

	sphere->numVertices = numVertices;

	std::unique_ptr<SimpleVertex[]> sphereVertices(new SimpleVertex[numVertices]);

	for (int slice = 0; slice <= numSlices; slice++)
	{
		float v = (float)slice / (float)numSlices;
		float inclination = v * XM_PI;
		float y = cos(inclination);
		float r = sin(inclination);
		for (int segment = 0; segment <= numSegments; segment++)
		{
			float u = (float)segment / (float)numSegments;
			float azimuth = u * XM_2PI;
			int index = slice * (numSegments + 1) + segment;
			sphereVertices[index].Pos = XMFLOAT3(r * sin(azimuth), y, r * cos(azimuth));
			sphereVertices[index].Normal = sphereVertices[index].Pos;
			sphereVertices[index].TexCoord = XMFLOAT2(u, v);
		}
	}

	const int numIndices = numSlices * (numSegments - 2) * 6;
	sphere->numIndices = numIndices;

	std::unique_ptr<WORD[]> sphereIndices(new WORD[numIndices]);

	unsigned int index = 0;

	for (int slice = 0; slice < numSlices; slice++)
	{
		unsigned short sliceBase0 = (unsigned short)((slice)*(numSegments + 1));
		unsigned short sliceBase1 = (unsigned short)((slice + 1)*(numSegments + 1));

		for (int segment = 0; segment < numSegments; segment++)
		{
			if (slice > 0)
			{
				sphereIndices[index++] = sliceBase0 + segment;
				sphereIndices[index++] = sliceBase0 + segment + 1;
				sphereIndices[index++] = sliceBase1 + segment + 1;
			}
			if (slice < numSlices - 1)
			{
				sphereIndices[index++] = sliceBase0 + segment;
				sphereIndices[index++] = sliceBase1 + segment + 1;
				sphereIndices[index++] = sliceBase1 + segment;
			}
		}
	}

	createBuffers(sphere, sphereVertices.get(), numVertices, sphereIndices.get(), numIndices);

	return sphere;
}

Mesh* D3D11GeometryFactory::createReferenceAxis()
{
	SimpleVertex axisVertices[] =
	{
		{ XMFLOAT3(0.500f, 0.000f, 0.000f), XMFLOAT3(0.125f, 0.500f, 0.500f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT3(0.125f, 0.500f, 0.500f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT3(0.125f, 0.500f, 0.500f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.500f, 0.000f, 0.000f), XMFLOAT3(0.125f, -0.500f, 0.500f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT3(0.125f, -0.500f, 0.500f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT3(0.125f, -0.500f, 0.500f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.500f, 0.000f, 0.000f), XMFLOAT3(0.125f, -0.500f, -0.500f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT3(0.125f, -0.500f, -0.500f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT3(0.125f, -0.500f, -0.500f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.500f, 0.000f, 0.000f), XMFLOAT3(0.125f, 0.500f, -0.500f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT3(0.125f, 0.500f, -0.500f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT3(0.125f, 0.500f, -0.500f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT2(0.250f, 0.250f) },
		{ XMFLOAT3(-0.500f, 0.000f, 0.000f), XMFLOAT3(-0.125f, 0.500f, 0.500f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT3(-0.125f, 0.500f, 0.500f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT3(-0.125f, 0.500f, 0.500f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(-0.500f, 0.000f, 0.000f), XMFLOAT3(-0.125f, 0.500f, -0.500f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT3(-0.125f, 0.500f, -0.500f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT3(-0.125f, 0.500f, -0.500f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(-0.500f, 0.000f, 0.000f), XMFLOAT3(-0.125f, -0.500f, -0.500f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT3(-0.125f, -0.500f, -0.500f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT3(-0.125f, -0.500f, -0.500f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(-0.500f, 0.000f, 0.000f), XMFLOAT3(-0.125f, -0.500f, 0.500f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT3(-0.125f, -0.500f, 0.500f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT3(-0.125f, -0.500f, 0.500f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT2(0.250f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.500f, 0.000f), XMFLOAT3(0.500f, 0.125f, 0.500f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT3(0.500f, 0.125f, 0.500f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT3(0.500f, 0.125f, 0.500f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.500f, 0.000f), XMFLOAT3(0.500f, 0.125f, -0.500f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT3(0.500f, 0.125f, -0.500f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT3(0.500f, 0.125f, -0.500f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.500f, 0.000f), XMFLOAT3(-0.500f, 0.125f, -0.500f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT3(-0.500f, 0.125f, -0.500f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT3(-0.500f, 0.125f, -0.500f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.500f, 0.000f), XMFLOAT3(-0.500f, 0.125f, 0.500f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT3(-0.500f, 0.125f, 0.500f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT3(-0.500f, 0.125f, 0.500f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT2(0.500f, 0.250f) },
		{ XMFLOAT3(0.000f, -0.500f, 0.000f), XMFLOAT3(0.500f, -0.125f, 0.500f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT3(0.500f, -0.125f, 0.500f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT3(0.500f, -0.125f, 0.500f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(0.000f, -0.500f, 0.000f), XMFLOAT3(-0.500f, -0.125f, 0.500f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT3(-0.500f, -0.125f, 0.500f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT3(-0.500f, -0.125f, 0.500f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(0.000f, -0.500f, 0.000f), XMFLOAT3(-0.500f, -0.125f, -0.500f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT3(-0.500f, -0.125f, -0.500f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT3(-0.500f, -0.125f, -0.500f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(0.000f, -0.500f, 0.000f), XMFLOAT3(0.500f, -0.125f, -0.500f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT3(0.500f, -0.125f, -0.500f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT3(0.500f, -0.125f, -0.500f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT2(0.500f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.500f), XMFLOAT3(0.500f, 0.500f, 0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT3(0.500f, 0.500f, 0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT3(0.500f, 0.500f, 0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.500f), XMFLOAT3(-0.500f, 0.500f, 0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT3(-0.500f, 0.500f, 0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT3(-0.500f, 0.500f, 0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.500f), XMFLOAT3(-0.500f, -0.500f, 0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT3(-0.500f, -0.500f, 0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT3(-0.500f, -0.500f, 0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, 0.500f), XMFLOAT3(0.500f, -0.500f, 0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT3(0.500f, -0.500f, 0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT3(0.500f, -0.500f, 0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT3(0.000f, 0.000f, -0.125f), XMFLOAT2(0.750f, 0.250f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.500f), XMFLOAT3(0.500f, 0.500f, -0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT3(0.500f, 0.500f, -0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT3(0.500f, 0.500f, -0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.500f), XMFLOAT3(0.500f, -0.500f, -0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT3(0.500f, -0.500f, -0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT3(0.500f, -0.500f, -0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.500f), XMFLOAT3(-0.500f, -0.500f, -0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT3(-0.500f, -0.500f, -0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT3(-0.500f, -0.500f, -0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.000f, -0.500f), XMFLOAT3(-0.500f, 0.500f, -0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT3(-0.500f, 0.500f, -0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT3(-0.500f, 0.500f, -0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(0.000f, 0.125f, 0.000f), XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(-0.125f, 0.000f, 0.000f), XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(0.000f, -0.125f, 0.000f), XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT2(0.750f, 0.500f) },
		{ XMFLOAT3(0.125f, 0.000f, 0.000f), XMFLOAT3(0.000f, 0.000f, 0.125f), XMFLOAT2(0.750f, 0.500f) },
	};

	WORD axisIndices[] =
	{
		0, 2, 1,
		3, 5, 4,
		6, 8, 7,
		9, 11, 10,
		12, 14, 13,
		12, 15, 14,
		16, 18, 17,
		19, 21, 20,
		22, 24, 23,
		25, 27, 26,
		28, 30, 29,
		28, 31, 30,
		32, 34, 33,
		35, 37, 36,
		38, 40, 39,
		41, 43, 42,
		44, 46, 45,
		44, 47, 46,
		48, 50, 49,
		51, 53, 52,
		54, 56, 55,
		57, 59, 58,
		60, 62, 61,
		60, 63, 62,
		64, 66, 65,
		67, 69, 68,
		70, 72, 71,
		73, 75, 74,
		76, 78, 77,
		76, 79, 78,
		80, 82, 81,
		83, 85, 84,
		86, 88, 87,
		89, 91, 90,
		92, 94, 93,
		92, 95, 94,
	};

	Mesh* axisMesh = new Mesh();
	axisMesh->name = L"Reference axis";
	axisMesh->numVertices = ARRAYSIZE(axisVertices);
	axisMesh->numIndices = ARRAYSIZE(axisIndices);
	axisMesh->primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	this->createBuffers(axisMesh, axisVertices, axisMesh->numVertices, axisIndices, axisMesh->numIndices);

	return axisMesh;
}

Mesh* D3D11GeometryFactory::createFromAiMesh(aiMesh* mesh)
{
	Mesh* demoMesh = new Mesh();

	demoMesh->numVertices = mesh->mNumVertices;
	demoMesh->numIndices = 0;

	std::unique_ptr<SimpleVertex[]> meshVertices(new SimpleVertex[demoMesh->numVertices]);

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		meshVertices[i].Pos.x = mesh->mVertices[i].x;
		meshVertices[i].Pos.y = mesh->mVertices[i].y;
		meshVertices[i].Pos.z = mesh->mVertices[i].z;

		if (mesh->HasNormals())
		{
			meshVertices[i].Normal.x = mesh->mNormals[i].x;
			meshVertices[i].Normal.y = mesh->mNormals[i].y;
			meshVertices[i].Normal.z = mesh->mNormals[i].z;
		}

		if (mesh->HasTextureCoords(0))
		{
			// TODO: handle multiple texcoords
			meshVertices[i].TexCoord.x = mesh->mTextureCoords[0]->x;
			meshVertices[i].TexCoord.y = mesh->mTextureCoords[0]->y;
		}

		if (mesh->HasTangentsAndBitangents()) {
			meshVertices[i].Binormal.x = mesh->mBitangents[i].x;
			meshVertices[i].Binormal.y = mesh->mBitangents[i].y;
			meshVertices[i].Binormal.z = mesh->mBitangents[i].z;

			meshVertices[i].Tangent.x = mesh->mTangents[i].x;
			meshVertices[i].Tangent.y = mesh->mTangents[i].y;
			meshVertices[i].Tangent.z = mesh->mTangents[i].z;
		}
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		demoMesh->numIndices += mesh->mFaces[i].mNumIndices;
	}

	std::unique_ptr<WORD[]> meshIndices(new WORD[demoMesh->numIndices]);

	UINT currentIndex = 0;

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		for (UINT vertexIndex = 0; vertexIndex < mesh->mFaces[i].mNumIndices; vertexIndex++)
		{
			meshIndices[currentIndex++] = mesh->mFaces[i].mIndices[vertexIndex];
		}
	}

	this->createBuffers(demoMesh, meshVertices.get(), demoMesh->numVertices, meshIndices.get(), demoMesh->numIndices);

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	
	demoMesh->name = converter.from_bytes(mesh->mName.C_Str());
	demoMesh->primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	return demoMesh;
}

void D3D11GeometryFactory::createBuffers(
	Mesh* mesh,
	SimpleVertex* vertices,
	int numVertices,
	WORD* indices,
	int numIndices
	) {
	// TODO: use ComPtr to prevent memory leak during errors.
	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	D3D11_BUFFER_DESC indexBufferDesc = { 0 };

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * numVertices;             // size is the VERTEX struct * 3
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	D3D11_SUBRESOURCE_DATA InitData = { 0 };
	InitData.pSysMem = vertices;

	DirectxHelper::ThrowIfFailed(dev->CreateBuffer(&vertexBufferDesc, &InitData, &mesh->vertexBuffer));
	SetDebugObjectName(mesh->vertexBuffer.Get(), "Vertext buffer");

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(WORD) * numIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;

	SecureZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;

	DirectxHelper::ThrowIfFailed(dev->CreateBuffer(&indexBufferDesc, &InitData, &mesh->indexBuffer));
	SetDebugObjectName(mesh->indexBuffer.Get(), "Index buffer");
}
