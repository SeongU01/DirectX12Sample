#include "pch.h"
#include "VIBuffer.h"
#include "d3dUtil.h"
void VIBuffer::Initialize(const VIBuffer::Descriptor& descriptor)
{
    Global::device->CreateVertexBuffer(descriptor.vertexData, descriptor.vertexSize, descriptor.vertexStride,
                                       _vertexBuffer, _vertexBufferView);
    Global::device->CreateIndexBuffer(descriptor.indexData, descriptor.indexSize, DXGI_FORMAT_R32_UINT, _indexBuffer,
                                      _indexBufferView);
    _indexCount = descriptor.indexCount;
    _vertexCount = descriptor.vertexSize / descriptor.vertexStride;
}

void VIBuffer::Initialize(ID3D12GraphicsCommandList* commandList, const VIBuffer::Descriptor& descriptor)
{
    Global::device->CreateVertexBuffer(commandList, descriptor.vertexData, descriptor.vertexSize,
                                       descriptor.vertexStride, _vertexBuffer, _vertexBufferView);

    Global::device->CreateIndexBuffer(commandList, descriptor.indexData, descriptor.indexSize, DXGI_FORMAT_R32_UINT,
                                      _indexBuffer, _indexBufferView);

    _indexCount  = descriptor.indexCount;
    _vertexCount = descriptor.vertexSize / descriptor.vertexStride;
}

void VIBuffer::DrawIndexedInstanced(ID3D12GraphicsCommandList* commandList, UINT instanceCount)
{
    commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
    commandList->IASetIndexBuffer(&_indexBufferView);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->DrawIndexedInstanced(_indexCount, instanceCount, 0, 0, 0);
}
