cbuffer ConstantBuffer : register(b0)
{
	matrix Model;
	matrix ModelInverseTranspose;
	matrix View;
	matrix Projection;

	float4 EyePosition;

	float4 OmniLightPositions[16];
	float4 OmniLightColors[16];
	matrix OmniLightRangePacked;

	uint   OmniLightCount;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float2 texcoord : TEXCOORD0;
	float3 binormal : BINORMAL;
	float3 tangent  : TANGENT;
};

struct VS_OUTPUT
{
	float4 position      : SV_POSITION;
	float3 normal        : NORMAL;
	float2 texcoord      : TEXCOORD0;
	float3 worldPosition : TEXCOORD1;
};

Texture2D microfacetTexture;
SamplerState microfacetSampler;

static float OmniLightRanges[16] = (float[16])OmniLightRangePacked;

VS_OUTPUT VShader(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	float4 temp = float4(input.position, 1.0f);
		temp = mul(temp, Model);

	output.worldPosition = temp.xyz / temp.w;

	temp = mul(temp, View);
	temp = mul(temp, Projection);

	output.position = temp;
	output.texcoord = input.texcoord;
	output.normal = mul(float4(input.normal, 0.0f), ModelInverseTranspose).xyz;

	return output;
}

float4 PShader(VS_OUTPUT input) : SV_TARGET
{
	uint i;

	float2 microfacetTextureUV;

	float3 finalColor = (float3)0;

	float3 viewDirection = normalize((float3)EyePosition - input.worldPosition);
		
	for (i = 0; i < OmniLightCount; i++)
	{
		float3 lightPosition    = OmniLightPositions[i].xyz;
		float3 lightAttenuation = float3(1.0, 0.02, 0.0);  //OmniLightAttenuations[i].xyz;
		float2 range            = (float2)OmniLightRanges[i];
		float3 lightDirection   = normalize(lightPosition - input.worldPosition);
		float  lightDistance    = distance(lightPosition, input.worldPosition);
		float3 halfway          = normalize(lightDirection + viewDirection);
		//microfacetTextureUV.x = halfway.z * range.x + range.y;
		//microfacetTextureUV.y = dot(lightDirection, halfway);
		
		//float3 color = microfacetTexture.Sample(microfacetSampler, microfacetTextureUV);
		//color *= saturate(min(viewDirection.z, lightDirection.z) * 2.0 * halfway.z / microfacetTextureUV.y);
		//finalColor += color * specularColor + diffuseColor * lightDirection.z;

		float3 lightColor = 
			(
				OmniLightColors[i].rgb // C0
				/
				(lightAttenuation.x + lightAttenuation.y * lightDistance + lightAttenuation.z * lightDistance * lightDistance)
			);
		
		// diffuse reflection
		finalColor += lightColor * saturate(dot(input.normal, lightDirection));
		// white specular reflection
		finalColor += float3(1.0, 1.0, 1.0) * pow(saturate(dot(input.normal, halfway)), 60);
	}

	return saturate(float4(finalColor, 1.0));
}


// Input control point
struct VS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : WORLDPOS;
	float2 vUV       : TEXCOORD0;
	float3 vTangent  : TANGENT;
};

// Output control point
struct BEZIER_CONTROL_POINT
{
	float3 vPosition    : BEZIERPOS;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float Edges[3]        : SV_TessFactor;
	float Inside[2]       : SV_InsideTessFactor;

	float3 vTangent[3]    : TANGENT;
	float2 vUV[3]         : TEXCOORD;
	float3 vTanUCorner[3] : TANUCORNER;
	float3 vTanVCorner[3] : TANVCORNER;
	float4 vCWts          : TANWEIGHTS;
};

#define MAX_POINTS 3

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT SubDToBezierConstantsHS(
	InputPatch<VS_OUTPUT, MAX_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	// Insert code to compute Output here

	return Output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("SubDToBezierConstantsHS")]
VS_CONTROL_POINT_OUTPUT SubDToBezierHS(
	InputPatch<VS_OUTPUT, MAX_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID)
{
	VS_CONTROL_POINT_OUTPUT Output;

	// Insert code to compute Output here.

	return Output;
}
