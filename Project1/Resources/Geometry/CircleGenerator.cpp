#include "pch.h"
#include "CircleGenerator.h"
#include "Global.h"
#include <cmath>

TArray<FVertexData> CircleGenerator::MakeCircle(int segmentCount, float r, float g, float b, float a)
{
    TArray<FVertexData> vertices;
    vertices.Reserve(segmentCount * 3);

    for (int i = 0; i < segmentCount; ++i)
    {
        float theta1 = (2.0f * Global::PI * i) / segmentCount;
        float theta2 = (2.0f * Global::PI * (i+1)) / segmentCount;
        
        // 중심점
        vertices.Add({ 0.0f, 0.0f, 0.0f, r, g, b, a });
        
        // 다음 둘레점
        vertices.Add({ cosf(theta2), sinf(theta2), 0.0f, r, g, b, a });

        // 현재 둘레점
        vertices.Add({ cosf(theta1), sinf(theta1), 0.0f, r, g, b, a });
    }


    return vertices;
}
