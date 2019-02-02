//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer cbNeverChanges : register(b0)
{
	matrix Projection;

	float4 AmbientRate;// ���ʶԻ�����ķ�����
	float4 DiffuseRate;//���ʶ��������ķ�����
	float4 SpecularRate;//���ʶԾ����ķ�����
	float Power;//���ʵľ���ⷴ��ϵ��
};


cbuffer cbChangesEveryFrame : register(b1)
{
	matrix World;
};

cbuffer cbRareChange: register(b2)
{
	matrix View;
	float4 EyePosition;
};

cbuffer LightBuffer: register(b3)
{
	//int type;
	float4 lightPosition;
	float4 lightDirection;
	float4 lightAmbient;//ǿ��
	float4 lightDiffuse;
	float4 lightSpecular;
	/*
	float4 lightAtt0;//����˥������
	float4 lightAtt1;//һ��˥������
	float4 lightAtt2;//����˥������
	float  lightAlpha;//��׶�Ƕ�
	float  lightBeta;//��׶�Ƕ�
	float  lightFailOff;//˥��ϵ��*/
};





//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD;
	float3 Norm: NORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
	float3 Norm: TEXCOORD1;
	float4 ViewDirection : TEXCOORD2;
	float4 LightVector : TEXCOORD3;


};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex = input.Tex;
    
	output.Norm = mul(input.Norm,(float3x3)World);
	output.Norm = normalize(output.Norm);

	float4 worldPos = mul(input.Pos,World);
	output.ViewDirection = EyePosition - worldPos;
	output.ViewDirection = normalize(output.ViewDirection);

	output.LightVector = lightPosition - worldPos;
	output.LightVector = normalize(output.LightVector);
	output.LightVector.w = length(lightPosition - worldPos);


    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 finalColor;
	float4 ambientColor, diffuseColor, specularColor;
	float3 lightVector = -lightDirection.xyz;
	lightVector = normalize(lightVector);

	ambientColor = float4(1.0f, 1.0f, 1.0f, 1.0f) * lightAmbient;

	float diffuseFactor = dot(lightVector,input.Norm);
	if (diffuseFactor > 0.0f)
	{
		diffuseColor = float4(1.0f,1.0f,1.0f,1.0f) * lightDiffuse * diffuseFactor;
		
		float3 reflection = reflect(-lightVector,input.Norm);
		float specularFactor = pow(max(dot(reflection,input.ViewDirection.xyz),0.0f),Power);
		specularColor = float4(1.0f, 1.0f, 1.0f, 1.0f) * lightSpecular * specularFactor;

	}
	finalColor = saturate(ambientColor + diffuseColor);// +specularColor);


	return txDiffuse.Sample(samLinear, input.Tex)*0.5+ 0.5*finalColor;// *vMeshColor;
}

/*Technique T0
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0,VS()));
		SetPixelShader(CompileShader(ps_5_0,PS()));
	};
}*/
