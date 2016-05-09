cbuffer ConstantBuffer
{
	matrix Model;
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
	output.normal = mul(float4(input.normal, 0.0f), Model).xyz;

	return output;
}

float4 PShader(VS_OUTPUT input) : SV_TARGET
{
	uint i;

	float3 finalColor = (float3)0;

	float3 viewDirection = normalize((float3)EyePosition - input.worldPosition);
		
	for (i = 0; i < OmniLightCount; i++)
	{
		/*float3 lightPosition = OmniLightPositions[i].xyz;
		float3 lightDirWorld = normalize(lightPosition - input.worldPosition);
		float NdotL = dot(input.normal, lightDirWorld);
		float Kd = saturate(NdotL);
		finalColor += (OmniLightColors[i] * Kd);
		*/

		float3 lightPosition = OmniLightPositions[i].xyz;
		float3 lightDirection = normalize(lightPosition - input.worldPosition);
		float3 lightColor = OmniLightColors[i].rgb;
		float lightRange = saturate(1 - dot(lightDirection / OmniLightRanges[i], lightDirection / OmniLightRanges[i]));
		float lightIntensity = saturate(dot(input.normal, lightDirection));
		float shadow = saturate(4.0 * lightRange);
		float3 reflection = normalize(2.0 * lightIntensity * input.normal - lightDirection);
		float specular = min(pow(saturate(dot(reflection, viewDirection)), 3), 1.0);

		//finalColor += shadow * saturate((lightColor * lightIntensity) * lightRange);
		finalColor += saturate(lightColor * lightIntensity);
	}

	return saturate(float4(finalColor, 1.0));
}
