#pragma once
#include <xnamath.h>
struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
	XMFLOAT3 Norm;
};


struct CBNeverChanges
{
	XMMATRIX mProjection;
	XMMATRIX mLightView;
	XMMATRIX mLightProj;

	XMFLOAT4 ambient;
	XMFLOAT4 specular;
	XMFLOAT4 diffuse;
	float    power;//镜面光反射系数
};

struct CBChangesEveryFrame
{
	XMMATRIX mWorld;

};

struct CBRareChanges
{
	XMMATRIX mView;
	XMVECTOR mEyePos;
	int flag;
};

struct Light
{
	//INT light_type;

	XMFLOAT4 pos;
	XMFLOAT4 direction;
	XMFLOAT4 _ambient;
	XMFLOAT4 _diffuse;
	XMFLOAT4 _specular;

	/*float	attenuation0;//常量衰减因子
	float	attenuation1;//一次衰减因子
	float	attenuation2;//二次衰减因子

	float	alpha;//内锥角度
	float	beta;//外锥角度
	float	fallOff;//衰减系数*/



};



