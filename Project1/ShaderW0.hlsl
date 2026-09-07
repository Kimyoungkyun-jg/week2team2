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
    float3 worldPos : TEXCOORD0;
};

PS_INPUT mainVS(VS_INPUT input) // Vertex Shader
{
    PS_INPUT output;
    
    //월드 변환 좌표 저장
    float4 worldPos = mul(input.position, World);
    output.worldPos = worldPos.xyz;
    
    //최종 화면 클립 좌표 변환
    output.position = mul(worldPos, VP);
    
    //CustomColor 적용
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
    //화면 편미분 기반 법선 복원
    float3 ddxPos = ddx(input.worldPos);
    float3 ddyPos = ddy(input.worldPos);
    float3 normal = cross(ddxPos, ddyPos);
    float normalLen = length(normal);
    
    //선(Line)이나 법선이 없는 경우 기존 색상 유지
    if (normalLen < 1e-5f)
    {
        return input.color;
    }
    normal /= normalLen;

    //조명 방향 설정
    float3 lightDir = normalize(float3(0.5f, 1.0f, -0.6f));
    
    //디퓨즈 음영 계산
    float NdotL = saturate(dot(normal, lightDir));
    float diffuse = NdotL * 0.65f + 0.35f;
    
    float3 baseColor = input.color.rgb;

    //하이라이트 대상 발광 및 광택 처리
    if (CustomColor.a > 0.0f)
    {
        //반사 하이라이트 (스펙큘러)
        float3 viewDir = normalize(float3(0.0f, 0.0f, -1.0f));
        float3 halfVec = normalize(lightDir + viewDir);
        float spec = pow(saturate(dot(normal, halfVec)), 20.0f);
        
        //외곽 림 라이트 발광
        float rim = 1.0f - saturate(dot(normal, viewDir));
        rim = pow(rim, 2.5f);
        
        //기본 음영 + 스펙큘러 + 발광 더하기
        float3 glowColor = baseColor * diffuse + float3(1.0f, 1.0f, 1.0f) * spec * 0.8f + baseColor * rim * 0.6f;
        return float4(glowColor, input.color.a);
    }

    //일반 오브젝트 음영 렌더링
    return float4(baseColor * diffuse, input.color.a);
}