#include "ViewManager.h"
#include "pch.h"

const UINT64 ViewManager::GetVertexBufferSrvPtr()
{
    UINT64 heapStartPtr = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart().ptr;
    UINT64 returnValue  = heapStartPtr + (_vertexSrvStartIndex * _shaderResourceDescriptorSize);
    return returnValue;
}

const UINT64 ViewManager::GetIndexBufferSrvPtr()
{
    UINT64 heapStartPtr = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart().ptr;
    UINT64 returnValue  = heapStartPtr + (_indexSrvStartIndex * _shaderResourceDescriptorSize);
    return returnValue;
}

void ViewManager::Initialize()
{
    HRESULT                    hr     = S_OK;
    ID3D12Device*              device = Global::device->GetDevice();
    D3D12_DESCRIPTOR_HEAP_DESC desc{};

    desc.NumDescriptors = ShaderResourceHeapCapacity;
    desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_shaderResourceHeap));
    FAILED_CHECK_MESSAGE(hr, L"Failed to create shader resource descriptor heap.");

    desc.NumDescriptors = RenderTargetHeapCapacity;
    desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_renderTargetHeap));
    FAILED_CHECK_MESSAGE(hr, L"Failed to create render target descriptor heap.");

    desc.NumDescriptors = DepthStencilHeapCapacity;
    desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_depthStencilHeap));
    FAILED_CHECK_MESSAGE(hr, L"Failed to create depth stencil descriptor heap.");

    _shaderResourceDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    _renderTargetDescriptorSize   = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    _depthStencilDescriptorSize   = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void ViewManager::AddDescriptorHeap(const ViewManager::Type type, D3D12_CPU_DESCRIPTOR_HANDLE& handle, UINT* ID)
{
    UINT offset = 0;

    switch (type)
    {
    case ViewManager::Type::SHADER_RESOURCE: {
        UINT index = 0;
        if (!_availableShaderResourceIDs.empty())
        {
            index = _availableShaderResourceIDs.front();
            _availableShaderResourceIDs.pop();
        }
        else
        {
            if (_numShaderResource >= _vertexSrvStartIndex)
            {
                throw std::out_of_range("Shader resource descriptor heap is full");
            }
            index = _numShaderResource++;
        }

        offset = _shaderResourceDescriptorSize * index;
        handle = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += offset;

        if (ID)
        {
            *ID = index;
        }
        break;
    }
    case ViewManager::Type::RENDER_TARGET:
        if (_numRenderTarget >= RenderTargetHeapCapacity)
        {
            throw std::out_of_range("Render target descriptor heap is full");
        }
        offset = _renderTargetDescriptorSize * _numRenderTarget++;
        handle = _renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += offset;
        break;

    case ViewManager::Type::DEPTH_STENCIL:
        if (_numDepthStencil >= DepthStencilHeapCapacity)
        {
            throw std::out_of_range("Depth stencil descriptor heap is full");
        }
        offset = _depthStencilDescriptorSize * _numDepthStencil++;
        handle = _depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += offset;
        break;

    case ViewManager::Type::VERTEX_BUFFER_SHADER_RESOURCE:
        if (_vertexSrvStartIndex + _numVertexSrv >= _indexSrvStartIndex)
        {
            throw std::out_of_range("Vertex buffer descriptor region is full");
        }
        offset = _shaderResourceDescriptorSize * (_numVertexSrv++ + _vertexSrvStartIndex);
        handle = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += offset;
        break;

    case ViewManager::Type::INDEX_BUFFER_SHADER_RESOURCE:
        if (_indexSrvStartIndex + _numIndexSrv >= ShaderResourceHeapCapacity)
        {
            throw std::out_of_range("Index buffer descriptor region is full");
        }
        offset = _shaderResourceDescriptorSize * (_numIndexSrv++ + _indexSrvStartIndex);
        handle = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += offset;
        break;
    }
}

void ViewManager::AddDescriptorHeap(const ViewManager::Type type, DescriptorHandles& handle, UINT* ID)
{
    UINT offset = 0;

    switch (type)
    {
    case ViewManager::Type::SHADER_RESOURCE: {
        UINT index = 0;
        if (!_availableShaderResourceIDs.empty())
        {
            index = _availableShaderResourceIDs.front();
            _availableShaderResourceIDs.pop();
        }
        else
        {
            if (_numShaderResource >= _vertexSrvStartIndex)
            {
                throw std::out_of_range("Shader resource descriptor heap is full");
            }
            index = _numShaderResource++;
        }

        offset     = _shaderResourceDescriptorSize * index;
        handle.CPU = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart();
        handle.CPU.ptr += offset;
        handle.GPU = _shaderResourceHeap->GetGPUDescriptorHandleForHeapStart();
        handle.GPU.ptr += offset;

        if (ID)
        {
            *ID = index;
        }
        break;
    }
    case ViewManager::Type::RENDER_TARGET:
        if (_numRenderTarget >= RenderTargetHeapCapacity)
        {
            throw std::out_of_range("Render target descriptor heap is full");
        }
        offset     = _renderTargetDescriptorSize * _numRenderTarget++;
        handle.CPU = _renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
        handle.CPU.ptr += offset;
        break;

    case ViewManager::Type::DEPTH_STENCIL:
        if (_numDepthStencil >= DepthStencilHeapCapacity)
        {
            throw std::out_of_range("Depth stencil descriptor heap is full");
        }
        offset     = _depthStencilDescriptorSize * _numDepthStencil++;
        handle.CPU = _depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
        handle.CPU.ptr += offset;
        break;

    case ViewManager::Type::VERTEX_BUFFER_SHADER_RESOURCE:
        if (_vertexSrvStartIndex + _numVertexSrv >= _indexSrvStartIndex)
        {
            throw std::out_of_range("Vertex buffer descriptor region is full");
        }
        offset     = _shaderResourceDescriptorSize * (_numVertexSrv++ + _vertexSrvStartIndex);
        handle.CPU = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart();
        handle.CPU.ptr += offset;
        break;

    case ViewManager::Type::INDEX_BUFFER_SHADER_RESOURCE:
        if (_indexSrvStartIndex + _numIndexSrv >= ShaderResourceHeapCapacity)
        {
            throw std::out_of_range("Index buffer descriptor region is full");
        }
        offset     = _shaderResourceDescriptorSize * (_numIndexSrv++ + _indexSrvStartIndex);
        handle.CPU = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart();
        handle.CPU.ptr += offset;
        break;
    }
}

void ViewManager::ReturnShaderResourceDescriptorHeap(UINT ID)
{
    _availableShaderResourceIDs.push(ID);
}

void ViewManager::ReturnShaderResourceDescriptorHeap(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
    if (_shaderResourceDescriptorSize == 0)
    {
        return;
    }

    const UINT64 heapStart = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart().ptr;
    if (handle.ptr < heapStart)
    {
        return;
    }

    const UINT64 offset = handle.ptr - heapStart;
    if (offset % _shaderResourceDescriptorSize != 0)
    {
        return;
    }

    const UINT id = static_cast<UINT>(offset / _shaderResourceDescriptorSize);
    if (id == 0 || id >= _numShaderResource || id >= _vertexSrvStartIndex)
    {
        return;
    }

    _availableShaderResourceIDs.push(id);
}
