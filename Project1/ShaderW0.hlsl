cbuffer ObjectConstants : register(b0) // FConstants
{
    matrix World;
};
cbuffer FrameConstants : register(b1) // FFrameConstants
{
    matrix VP;
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

PS_INPUT mainVS(VS_INPUT input) // Vertex Shader (Color)
{
    PS_INPUT output;
    
    output.position = mul(mul(input.position, World), VP);
    output.color = input.color;
    
    return output;
}

PS_INPUT mainSimpleVS(VS_INPUT_SIMPLE input) // Vertex Shader (Position only)
{
    PS_INPUT output;
    
    output.position = mul(mul(input.position, World), VP);
    output.color = float4(1.0f, 0.4f, 0.4f, 1.0f);
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET // Pixel Shader
{
    return input.color;
}