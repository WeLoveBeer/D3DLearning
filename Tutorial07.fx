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
	int type;
	//float4 lightPosition;
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
    float2 Tex : TEXCOORD0;
	//float3 Norm: NORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
	//float3 Norm: TEXCOORD1;
	//float4 ViewDirection : TEXCOORD2;
	//float4 LightVector : TEXCOORD3;


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
    
	/*output.Norm = mul(input.Norm,(float3x3)World);
	output.Norm = normalize(output.Norm);

	float worldPos = mul(input.Pos,World);
	output.ViewDirection = Ey*/

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	return txDiffuse.Sample(samLinear, input.Tex);// *vMeshColor;
}

/*Technique T0
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0,VS()));
		SetPixelShader(CompileShader(ps_5_0,PS()));
	};
}*/
