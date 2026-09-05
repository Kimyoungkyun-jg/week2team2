#include "pch.h"
#include "ConstBuffer.h"

ConstBuffer::ConstBuffer(void* data, uint32 dataSize) : data(data), dataSize(dataSize)
{
	D3D11_BUFFER_DESC constantbufferdesc = {};
	constantbufferdesc.ByteWidth = sizeof(FConstants) + 0xf & 0xfffffff0;
	constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DEVICE->CreateBuffer(&constantbufferdesc, nullptr, &buffer);
}

ConstBuffer::~ConstBuffer()
{
	buffer->Release();
}

void ConstBuffer::Update()
{
	DC->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy(subResource.pData, data, dataSize);
	DC->Unmap(buffer, 0);
}

void ConstBuffer::SetVSBuffer(UINT slot)
{
	Update();
	DC->VSSetConstantBuffers(slot, 1, &buffer);
}
