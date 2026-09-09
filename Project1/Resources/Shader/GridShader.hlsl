cbuffer ObjectConstants : register(b0)
{
    matrix World;
};

cbuffer FrameConstants : register(b1)
{
    matrix VP;
    float3 CameraPos;
    float pad;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
};

PS_INPUT mainVS_Grid(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    float4 worldPos = mul(float4(input.position, 1.0f), World);

    output.position = mul(worldPos, VP);
    output.worldPosition = worldPos.xyz;

    return output;
}

float4 mainPS_Grid(PS_INPUT input) : SV_TARGET
{
    int tileX = (int) floor(input.worldPosition.x);
    int tileZ = (int) floor(input.worldPosition.z);

    if ((tileX + tileZ) % 2 == 0)
        return float4(0.65f, 0.65f, 0.65f, 1.0f);

    return float4(0.45f, 0.45f, 0.45f, 1.0f);
}