cbuffer ObjectConstants : register(b0) // FConstants
{
    matrix World;
};
cbuffer FrameConstants : register(b1) // FFrameConstants
{
    matrix VP;
    float3 CameraPos; 
    float pad;
};
cbuffer ColorConstants : register(b2)
{
    float4 CustomColor;
    int UseTexture;
    float3 ColorPad;
};

// 텍스처 및 샘플러 레지스터
Texture2D MainTexture : register(t0);
SamplerState MainSampler : register(s0);

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
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};

// 일반 버텍스 셰이더
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 worldPos = mul(float4(input.position, 1.0f), World);
    output.position = mul(worldPos, VP);
    output.worldPosition = worldPos.xyz;
    output.uv = input.uv;
    output.normal = normalize(mul(input.normal, (float3x3)World));
    
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

// 일반 픽셀 셰이더
float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float4 finalColor = input.color;
    if (UseTexture != 0)
    {
        finalColor = MainTexture.Sample(MainSampler, input.uv);
    }
    return finalColor;
}

// 스카이스피어 전용 셰이더 구조체
struct PS_INPUT_SKY
{
    float4 position : SV_POSITION;
    float3 localPos : TEXCOORD0;
};



// 스카이스피어 전용 버텍스 셰이더
PS_INPUT_SKY mainVS_Sky(VS_INPUT input)
{
    PS_INPUT_SKY output;
    
    float4 clipPos = mul(mul(float4(input.position, 1.0f), World), VP);
    output.position = clipPos.xyww;
    output.localPos = input.position;
    
    return output;
}



// 스카이스피어 전용 픽셀 셰이더
float4 mainPS_Sky(PS_INPUT_SKY input) : SV_TARGET
{
    float3 dir = normalize(input.localPos);
    float u = 0.5f + atan2(dir.x, dir.z) / 6.2831853f;
    float v = 0.5f - asin(clamp(dir.y, -1.0f, 1.0f)) / 3.1415926f;
    
    return MainTexture.Sample(MainSampler, float2(u, v));
}




PS_INPUT mainVS_Outline(VS_INPUT input)
{
    PS_INPUT output;
    
    float outlinePixels = CustomColor.x;
    float screenWidth = CustomColor.y;
    float screenHeight = CustomColor.z;
    
    float3 norm = length(input.normal) > 0.001f ? input.normal : input.position;
    float len = length(norm);
    norm = (len > 0.0001f) ? (norm / len) : float3(0.0f, 1.0f, 0.0f);

    float4 clipPos = mul(mul(float4(input.position, 1.0f), World), VP);
    float4 clipNormal = mul(mul(float4(norm, 0.0f), World), VP);
    
    float2 offsetDir = normalize(clipNormal.xy + 0.00001f);
    float2 pixelToNdc = float2(outlinePixels * 2.0f / screenWidth, outlinePixels * 2.0f / screenHeight);
    
    clipPos.xy += offsetDir * pixelToNdc * clipPos.w;
    
    float4 worldPos = mul(float4(input.position, 1.0f), World);

    output.position = clipPos;
    output.worldPosition = worldPos.xyz;
    output.color = float4(1.0f, 1.0f, 0.0f, 1.0f);
    output.uv = input.uv;
    output.normal = norm;
    return output;
}