#include "pch.h"
#include "UGrid.h"
#include "FVertexSimple.h"

UGrid::~UGrid() {
  if (GridMesh) {
    delete GridMesh;
    GridMesh = nullptr;
  }
  if (WorldBuffer) {
    delete WorldBuffer;
    WorldBuffer = nullptr;
  }
}

void UGrid::Initialize() {
  CreateVertices();

  WorldBuffer = new MatrixBuffer();
  GridMesh = new Mesh(Vertices);
}

void UGrid::CreateVertices() {
  int HalfSize = GridSize / 2;

  for (int z = -HalfSize; z < HalfSize; ++z) {
    for (int x = -HalfSize; x < HalfSize; ++x) {
      float x0 = x * CellSize;
      float x1 = (x + 1) * CellSize;

      float z0 = z * CellSize;
      float z1 = (z + 1) * CellSize;

      // 첫번째 삼각형
      Vertices.Add(FVertexData{x0, 0.0f, z0});
      Vertices.Add(FVertexData{x0, 0.0f, z1});
      Vertices.Add(FVertexData{x1, 0.0f, z1});

      // 두번째 삼각형
      Vertices.Add(FVertexData{x0, 0.0f, z0});
      Vertices.Add(FVertexData{x1, 0.0f, z1});
      Vertices.Add(FVertexData{x1, 0.0f, z0});
    }
  }
}

void UGrid::Render() {
  if (!GridMesh)
    return;

  FMatrix World = FMatrix::Translation(Location);

  WorldBuffer->SetMat(World);
  WorldBuffer->SetVSBuffer(0);

  RENDERER.PrepareGridShader();
  GridMesh->IASet(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  DC->Draw(GridMesh->GetNumVertices(), 0);
}

void UGrid::Update(const FVector &CameraLocation) {
  Location.x = floor(CameraLocation.x / CellSize) * CellSize;
  Location.y = 0.0f;
  Location.z = floor(CameraLocation.z / CellSize) * CellSize;
}