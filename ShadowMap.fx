struct shadow_VS_INPUT
{
	float4 position : POSITION;
};
struct shadow_PS_INPUT
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXTURE0;
};

shadow_PS_INPUT shadow_VS(shadow_VS_INPUT input)
{
	input.position.w = 1.0f;//_
	shadow_PS_INPUT output;
	output.position = mul(input.position,worldMatrix);
	output.position = mul(output.position,viewMatrix);
	output.position = mul(output.position,projectionMatrix);

	output.depthPosition = output.position;
	return output;
}
float4 shadow_PS(shadow_PS_INPUT input) : SV_TARGET
{
	float depthValue = input.depthPosition.z / input.depthPosition.w;
	float4 color = float4(depthValue,depthValue,depthValue,1.0f);
	return color;
}