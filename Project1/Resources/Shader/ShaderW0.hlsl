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
    float3 worldPosition : TEXCOORD0;
};

// 일반 버텍스 셰이더
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    output.position = mul(mul(input.position, World), VP);
    
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
    return input.color;
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
    
    float4 clipPos = mul(mul(input.position, World), VP);
    output.position = clipPos.xyww;
    output.localPos = input.position.xyz;
    
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
    
    // 얼추맞는 값이기 때문에 100% 맞진 않음
    // 정확하게 하고 싶다면 normal 성분도 같이 올려보내야 함.
    float3 pseudoNormal = input.position.xyz;
    float len = length(pseudoNormal);
    pseudoNormal = (len > 0.0001f) ? (pseudoNormal / len) : float3(0.0f, 1.0f, 0.0f);

    float4 clipPos = mul(mul(input.position, World), VP);
    float4 clipNormal = mul(mul(float4(pseudoNormal, 0.0f), World), VP);
    
    float2 offsetDir = normalize(clipNormal.xy + 0.00001f);
    float2 pixelToNdc = float2(outlinePixels * 2.0f / screenWidth, outlinePixels * 2.0f / screenHeight);
    
    clipPos.xy += offsetDir * pixelToNdc * clipPos.w;
    
    float4 worldPos = mul(input.position, World);

    output.position = mul(worldPos, VP);
    output.worldPosition = worldPos.xyz;
    output.color = float4(1.0f, 1.0f, 0.0f, 1.0f);
    return output;
}