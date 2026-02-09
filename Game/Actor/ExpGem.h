#pragma once
#include "Actor\Actor.h"

using namespace Wanted;

class Player;

class ExpGem : public Actor
{
    RTTI_DECLARATIONS(ExpGem, Actor)

public:
    ExpGem(const Vector2& position);
    ~ExpGem();

private:
    virtual void Tick(float deltaTime) override;

    int exp = 3;
};

