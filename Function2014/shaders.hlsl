cbuffer ConstantBuffer 
{
	matrix World;
	matrix View;
	matrix Projection;

	float3 EyePosition;

	float3 LightDir[4];
	float4 LightColor[4];
	float  LightIntensity[4];

	float4 AmbientColor;
	float  AmbientIntensity;
}


struct VS_INPUT
{
	float4 position : POSITION;
	float3 normal   : NORMAL;
	float4 color    : COLOR0;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color    : COLOR0;
	float3 normal   : TEXCOORD0;
	float3 view     : TEXCOORD1;
};

VS_OUTPUT VShader(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	float4 worldPosition = mul(input.position, World);

	output.position = worldPosition;
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	output.normal = mul(input.normal, World);

	output.view = normalize(float4(EyePosition, 1.0) - worldPosition);

	output.color = input.color;

	return output;
}

float4 PShader(VS_OUTPUT input) : SV_TARGET
{
	float4 finalColor = AmbientIntensity * AmbientColor;

	float4 normal = float4(input.normal, 1.0);

	for (int i = 0; i < 2; i++)
	{
		float4 diffuse = saturate(dot(-LightDir[i], normal));
		float4 reflect = normalize(2 * diffuse*normal - float4(LightDir[i], 1.0));
		float4 specular = pow(saturate(dot(reflect, input.view)), 15);
		
		finalColor += diffuse * LightColor[i] * LightIntensity[i] + specular * LightColor[i] * LightIntensity[i];
	}

	return saturate(finalColor);
}
