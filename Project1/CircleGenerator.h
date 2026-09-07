#pragma once
#include "Containers.h"
#include "FVertexSimple.h"
class CircleGenerator
{
public:
    static TArray<FVertexColor> MakeCircle(int segmentCount, float r, float g, float b, float a);
};