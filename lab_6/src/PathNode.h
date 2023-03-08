#pragma once
#include "Agent.h"
class PathNode :
    public Agent
{
public:
    PathNode();
    ~PathNode() = default;

    virtual void Draw() override;
    virtual void Update() override;
    virtual void Clean() override;
};

