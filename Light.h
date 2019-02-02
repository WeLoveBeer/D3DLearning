#ifndef LIGHT_H
#define LIGHT_H
#include <xnamath.h>
struct Material
{
	//反射率
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
	XMFLOAT4 diffuse;
	float    power;//镜面光反射系数

};

struct Light1
{
	//float light_type;

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


#endif