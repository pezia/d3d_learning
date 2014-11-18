#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Function2014/Mesh.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Function2014Test
{

	TEST_CLASS(MechTests)
	{
	public:
		BEGIN_TEST_METHOD_ATTRIBUTE(DefaultValues)
			TEST_DESCRIPTION(L"Test mesh default values")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(DefaultValues)
		{
			Mesh mesh;

			Assert::IsNull(mesh.vertices, L"Vertices is initialized to NULL");
			Assert::IsNull(mesh.indices, L"Indices is initialized to NULL");

			Assert::IsNull(mesh.vertexBuffer, L"Vertex buffer is initialized to NULL");
			Assert::IsNull(mesh.indexBuffer, L"Index buffer is initialized to NULL");

			Assert::AreEqual(0, (int)mesh.vertexCount, L"Vertex count is zero");
			Assert::AreEqual(0, (int)mesh.indexCount, L"Index count is zero");
		}
	};
}
