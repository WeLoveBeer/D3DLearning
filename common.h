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
	float    power;//����ⷴ��ϵ��
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

	/*float	attenuation0;//����˥������
	float	attenuation1;//һ��˥������
	float	attenuation2;//����˥������

	float	alpha;//��׶�Ƕ�
	float	beta;//��׶�Ƕ�
	float	fallOff;//˥��ϵ��*/



};



