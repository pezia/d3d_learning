cbuffer ConstantBuffer
{
	matrix Model;
	matrix View;
	matrix Projection;

	float3 EyePosition;

	uint   LightCount;
	float3 LightDir[16];
	float4 LightColor[16];
	matrix LightIntensities;

	float4 AmbientColor;
	float  AmbientIntensity;
};

static float LightIntensity[16] = (float[16])LightIntensities;

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float2 texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position      : SV_POSITION;
	float3 normal        : NORMAL;
	float2 texcoord      : TEXCOORD0;
	float3 worldPosition : TEXCOORD1;
};

Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

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
	output.normal = mul(float4(input.normal, 0.0f), Model).xyz;

	return output;
}

float4 PShader(VS_OUTPUT input) : SV_TARGET
{
	float3 finalColor = 0;
	float3 normal = normalize(input.normal);

	for (int i = 0; i < LightCount; i++)
	{
		float3 lightDirWorld = mul(LightDir[i], Model);
		float NdotL = dot(normal, lightDirWorld);
		float Ka = saturate(NdotL + 1);
		float Kd = saturate(NdotL);
		finalColor += /*(AmbientColor * Ka) + */(LightColor[i] * Kd * LightIntensity[i]);
	}

	return saturate(float4(finalColor, 1.0f));
}
