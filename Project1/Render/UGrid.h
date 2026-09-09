#pragma once
#include "FVertexSimple.h"

class UGrid
{
public:
    UGrid() = default;
    ~UGrid();

    void Initialize();
    void Update(const FVector& CameraLocation);
    void Render();

    void CreateVertices();

private:
    float GridSize = 100.0f;
    float CellSize = 1.0f;

    TArray<FVertexData> Vertices;
    Mesh* GridMesh = nullptr;

    FVector Location = FVector(0, 0, 0);
    MatrixBuffer* WorldBuffer = nullptr;
};