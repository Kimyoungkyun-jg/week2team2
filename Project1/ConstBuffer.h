#pragma once

class ConstBuffer
{
protected:
    ConstBuffer(void* data, uint32 dataSize);
    ~ConstBuffer();

    void Update();

public:
    void SetVSBuffer(UINT slot);

private:
    ID3D11Buffer* buffer;

    void* data;
    uint32 dataSize;

    D3D11_MAPPED_SUBRESOURCE subResource;
};
