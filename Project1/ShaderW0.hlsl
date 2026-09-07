cbuffer ObjectConstants : register(b0) // FConstants
{
    matrix World;
};
cbuffer FrameConstants : register(b1) // FFrameConstants
{
    matrix VP;
};
cbuffer ColorConstants : register(b2)
{
    float4 CustomColor;
};

struct VS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VS_INPUT_SIMPLE
{
    float4 position : POSITION;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT mainVS(VS_INPUT input) // Vertex Shader
{
    PS_INPUT output;
    
    output.position = mul(mul(input.position, World), VP);
    
    // CustomColor.a가 0보다 크면 CustomColor(기즈모 축 색상) 사용, 아니면 정점 컬러 사용
    if (CustomColor.a > 0.0f)
    {
        output.color = CustomColor;
    }
    else
    {
        output.color = input.color;
    }
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET // Pixel Shader
{
    return input.color;
}