#pragma once
class VertexBuffer
{
public:
    VertexBuffer() : buffer(nullptr), stride(0), offset(0) {}
    VertexBuffer(const void* data, UINT stride, UINT count);
    ~VertexBuffer();

    void IASet(D3D11_PRIMITIVE_TOPOLOGY type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    void Update(const void* data, UINT count);

public:
    ID3D11Buffer* buffer = nullptr;
    UINT stride = 0;
    UINT offset = 0;
};

