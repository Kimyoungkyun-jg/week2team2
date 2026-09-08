#include "pch.h"
#include "Vertexbuffer.h"

VertexBuffer::VertexBuffer(const void* data, UINT stride, UINT count)
    : stride(stride), offset(0)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = stride * count;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA subData = {};
    subData.pSysMem = data;

    DEVICE->CreateBuffer(&bufferDesc, &subData, &buffer);
}

VertexBuffer::~VertexBuffer()
{
    if (buffer)
    {
        buffer->Release();
        buffer = nullptr;
    }
}

void VertexBuffer::IASet(D3D11_PRIMITIVE_TOPOLOGY type)
{
    DC->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
    DC->IASetPrimitiveTopology(type);
}
