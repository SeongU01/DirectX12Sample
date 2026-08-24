#pragma once
#include "Resource.h"

class BaseMesh;
class Model : public Resource
{
public:
    Model();
    virtual ~Model();

public:
    void AddMesh(std::unique_ptr<BaseMesh> mesh);

private:
    std::vector<std::unique_ptr<BaseMesh>> _meshes;
};
