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
    
    output.position = clipPos;
    output.color = float4(1.0f, 1.0f, 0.0f, 1.0f);
    return output;
}