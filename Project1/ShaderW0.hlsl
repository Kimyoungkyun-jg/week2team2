cbuffer constants : register(b0) // FConstants
{
    matrix worldmat;
    matrix viewmat;
    matrix projmat;
    float AspectRatio;
    float3 Pad;
};


struct VS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT mainVS(VS_INPUT input) // Vertex Shader
{
    PS_INPUT output;
    
    //// Scale
    //float2 scaled = input.position.xy * Scale.xy;
    
    //// Rotate
    //float cosTheta = cos(Rotation);
    //float sinTheta = sin(Rotation);
    //float2 rotated = float2(scaled.x * cosTheta - scaled.y * sinTheta, scaled.x * sinTheta + scaled.y * cosTheta);
    
    //// Offset
    //float2 translated = rotated + Offset.xy;
    
    output.position = mul(input.position, worldmat);
    output.position = mul(output.position, viewmat);
    output.position = mul(output.position, projmat);

    
    output.color = input.color;
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET // Pixel Shader
{
    return input.color;
}