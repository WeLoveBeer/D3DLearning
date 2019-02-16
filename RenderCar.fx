Texture2D txDiffuse : register( t0 );
Texture2D txShadowMap : register( t1 );
Texture2D txHeightMap : register( t2 );
SamplerState ClampSampleType : register( s0 );

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
	float4 lightPosition;
	float4 lightDirection;
	float4 lightAmbient;//强度
	float4 lightDiffuse;
	float4 lightSpecular;
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
	float4 ProjPos:POSITION;//平行光坐标下的位置
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
	float4 pos = input.Pos;
	if (flag!= 0 )
	{
		//pos.y = saturate(txHeightMap.Sample(ClampSampleType, input.Tex).r);
	}
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );

	pos = input.Pos;
	//pos.w = 1.0f;
	output.ProjPos = mul(pos,World);
	output.ProjPos = mul(output.ProjPos,LightView);
	output.ProjPos = mul(output.ProjPos,LightProj);


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

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 finalColor;
	float4 ambientColor, diffuseColor;
	float3 lightVector = -lightDirection.xyz;
	lightVector = normalize(lightVector);
	//计算ambient
	ambientColor = float4(1.0f, 1.0f, 1.0f, 1.0f) * lightAmbient;

	float diffuseFactor = dot(lightVector,input.Norm);
	if (diffuseFactor > 0.0f)
	{
		//计算diffuse
		diffuseColor = float4(1.0f,0.0f,0.0f,1.0f) * lightDiffuse * diffuseFactor;
	}
	/*处理阴影****************************************************************/
	float bias = 0.001f;
	float4 color =  ambientColor;
	float2 ShadowTex;
	float ShadowMapDepth;
	float Depth;
	ShadowTex.x = (input.ProjPos.x / input.ProjPos.w)*0.5f + 0.5f;
	ShadowTex.y = (input.ProjPos.y / input.ProjPos.w)*(-0.5f) + 0.5f;
	if (saturate(ShadowTex.x) == ShadowTex.x&&saturate(ShadowTex.y) == ShadowTex.y)
	{
		ShadowMapDepth = txShadowMap.Sample(ClampSampleType, ShadowTex).r;
		Depth = input.ProjPos.z / input.ProjPos.w;
		//处理ShadowMap上黑色的部分，因为ShadowMap只渲染了车顶，其余的都是黑色的
		if (ShadowMapDepth < 0.1)
			ShadowMapDepth = 0.9;
		ShadowMapDepth = ShadowMapDepth + bias;
		//如果比ShadowMap的深度小，就加上diffuseColor
		if (Depth < ShadowMapDepth )
		{
			color += diffuseColor;
			color = saturate(color);
		}
	}
	return txDiffuse.Sample(ClampSampleType, input.Tex) * color;
}

