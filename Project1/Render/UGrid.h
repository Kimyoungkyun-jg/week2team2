#pragma once
#include "FVertexSimple.h"

class UGrid
{
public:
    void Initialize();
    void Update(const FVector& CameraLocation);
    void Render();

    void CreateVertices();
    void CreateFloorVertices();

    void SetVisible(bool bVisible);
    void Toggle();
    bool IsVisible() const;

private:
    bool bVisible = true;

    float GridSize = 2000.0f;
    float CellSize = 1.0f;

    TArray<FVertexSimple> Vertices;
    Mesh* GridMesh = nullptr;

    FVector Location = FVector(0, 0, 0);
    MatrixBuffer* WorldBuffer = nullptr;
};