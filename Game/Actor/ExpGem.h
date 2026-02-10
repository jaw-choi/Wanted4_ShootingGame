#pragma once
#include "Actor\Actor.h"
#include <vector>

namespace Wanted { class Level; }
using namespace Wanted;

class Player;

class ExpGem : public Actor
{
    RTTI_DECLARATIONS(ExpGem, Actor)

public:
    ExpGem(const Vector2& position);
    ~ExpGem();

    static ExpGem* Acquire(Level* owner, const Vector2& position);
    static void Prewarm(Level* owner, int count);
    void ReleaseToPool();
    void Initialize(const Vector2& position);

private:
    virtual void Tick(float deltaTime) override;

    int exp = 3;

    bool inPool = false;

    static std::vector<ExpGem*> pool;
};

