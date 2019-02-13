//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
Texture2D txShadowMap : register( t1 );
Texture2D txHeightMap : register( t2 );
SamplerState ClampSampleType : register( s0 );
SamplerState  WrapSampleType : register( s1 );
//SamplerState ClampSampleType : register(s2);
cbuffer cbNeverChanges : register(b0)
{
	matrix Projection;
	matrix LightView;
	matrix LightProj;

	float4 AmbientRate;// 材质对环境光的反射率
	float4 DiffuseRate;//材质对漫反射光的反射率
	float4 SpecularRate;//材质对镜面光的反射率
	float Power;//材质的镜面光反射系数
};


cbuffer cbChangesEveryFrame : register(b1)
{
	matrix World;
};

cbuffer cbRareChange: register(b2)
{
	matrix View;
	float4 EyePosition;
	int flag;
};

cbuffer LightBuffer: register(b3)
{
	//int type;
	float4 lightPosition;
	float4 lightDirection;
	float4 lightAmbient;//强度
	float4 lightDiffuse;
	float4 lightSpecular;
	/*
	float4 lightAtt0;//常量衰减因子
	float4 lightAtt1;//一次衰减因子
	float4 lightAtt2;//二次衰减因子
	float  lightAlpha;//内锥角度
	float  lightBeta;//外锥角度
	float  lightFailOff;//衰减系数*/
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
	float4 ProjPos:POSITION;
    float2 Tex : TEXCOORD0;
	float3 Norm: TEXCOORD1;
	float4 ViewDirection : TEXCOORD2;
	float4 LightVector : TEXCOORD3;
	float4 Pos_W:POSITION1;


};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	float4 pos = input.Pos;
	if (flag!= 0 )
	{
		//pos.y = saturate(txHeightMap.Sample(ClampSampleType, input.Tex).r);
	}
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );

	output.ProjPos = mul(input.Pos,World);
	output.ProjPos = mul(output.Pos,LightView);
	output.ProjPos = mul(output.Pos,LightProj);


    output.Tex = input.Tex;
    
	output.Norm = normalize(input.Norm);
	output.Norm = mul(output.Norm,(float3x3)World);
	output.Norm = normalize(output.Norm);

	float4 worldPos = mul(input.Pos,World);
	output.ViewDirection = EyePosition - worldPos;
	output.ViewDirection = normalize(output.ViewDirection);

	output.LightVector = lightPosition - worldPos;
	output.LightVector = normalize(output.LightVector);
	output.LightVector.w = length(lightPosition - worldPos);

	output.Pos_W = mul(input.Pos, World);

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
		diffuseColor = float4(1.0f,0.0f,0.0f,1.0f) * lightDiffuse * diffuseFactor;
		
		float3 reflection = reflect(-lightVector,input.Norm);
		float specularFactor = pow(max(dot(reflection,input.ViewDirection.xyz),0.0f),Power);
		specularColor = float4(1.0f, 1.0f, 1.0f, 1.0f) * lightSpecular * specularFactor;

	}
	finalColor = saturate(ambientColor + diffuseColor);// +specularColor);
	/*****************************************************************/
	float bias = 0.001f;
	float4 color = { 0.0f,0.0f,0.0f,0.0f }; //
	float2 ShadowTex;
	float ShadowMapDepth;
	float Depth;
	ShadowTex.x = (input.ProjPos.x / input.ProjPos.w)*0.5f + 0.5f;
	ShadowTex.y = (input.ProjPos.y / input.ProjPos.w)*(-0.5f) + 0.5f;
	if (saturate(ShadowTex.x) == ShadowTex.x&&saturate(ShadowTex.y) == ShadowTex.y)
	{
		ShadowMapDepth = txShadowMap.Sample(ClampSampleType, ShadowTex).r;
		Depth = input.ProjPos.z / input.ProjPos.w;
		ShadowMapDepth = ShadowMapDepth + bias;
		if (ShadowMapDepth >= Depth)
		{
			color += finalColor;
			color = saturate(color);
		}

	}

	return saturate(txDiffuse.Sample(ClampSampleType, input.Tex)*finalColor);// *vMeshColor;
	//return txDiffuse.Sample(ClampSampleType, input.Tex) * color;// *vMeshColor;
	//return tex2D(ClampSampleType,float4(input.Tex.xy,0,0));
}

/*Technique T0
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0,VS()));
		SetPixelShader(CompileShader(ps_5_0,PS()));
	};
}*/
