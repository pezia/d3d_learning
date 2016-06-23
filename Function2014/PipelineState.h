#pragma once

#include "common.h"

using namespace Microsoft::WRL;

class PipelineState
{
public:
	D3D11_SAMPLER_DESC              samplerStateDesc;
	ComPtr<ID3D11SamplerState>      samplerState;

	D3D11_BLEND_DESC                blendStateDesc;
	ComPtr<ID3D11BlendState1>       blendState;

	D3D11_RASTERIZER_DESC1          rasterizerStateDesc;
	ComPtr<ID3D11RasterizerState1>  rasterizerState;

	D3D11_DEPTH_STENCIL_DESC        depthStencilStateDesc;
	ComPtr<ID3D11DepthStencilState> depthStencilState;

	D3D11_INPUT_ELEMENT_DESC*       inputElementDescriptor;
	UINT                            inputElementCount;
	ComPtr<ID3D11InputLayout>       inputLaoyut;
	D3D11_PRIMITIVE_TOPOLOGY        primitiveTopology;

	ComPtr<ID3D11PixelShader>       pixelShader;
	ComPtr<ID3D11VertexShader>      vertexShader;
	ComPtr<ID3D11DomainShader>      domainShader;
	ComPtr<ID3D11HullShader>        hullShader;
	ComPtr<ID3D11GeometryShader>    geometryShader;

	UINT                            renderTargetCount;
	DXGI_FORMAT                     renderTargetViewFormats[8];
	DXGI_FORMAT	                    depthSencilFormat;
	DXGI_SAMPLE_DESC                sampleDesc;

public:

	PipelineState() :
		samplerState(nullptr),
		blendState(nullptr),
		rasterizerState(nullptr),
		depthStencilState(nullptr),
		inputLaoyut(nullptr),
		pixelShader(nullptr),
		vertexShader(nullptr),
		domainShader(nullptr),
		hullShader(nullptr),
		geometryShader(nullptr)
	{
	}

	~PipelineState()
	{
		samplerState      = nullptr;
		blendState        = nullptr;
		rasterizerState   = nullptr;
		depthStencilState = nullptr;
		inputLaoyut       = nullptr;
		pixelShader       = nullptr;
		vertexShader      = nullptr;
		domainShader      = nullptr;
		hullShader        = nullptr;
		geometryShader    = nullptr;
		SAFE_DELETE_ARRAY(inputElementDescriptor);
	}
};
