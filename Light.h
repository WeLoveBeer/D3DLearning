#ifndef LIGHT_H
#define LIGHT_H
#include <xnamath.h>
struct Material
{
	//������
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
	XMFLOAT4 diffuse;
	float    power;//����ⷴ��ϵ��

};

struct Light1
{
	//float light_type;

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


#endif