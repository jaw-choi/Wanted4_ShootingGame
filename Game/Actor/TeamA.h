#pragma once

#include "Math/Rect.h"
#include "Actor/Actor.h"
#include "Util/Timer.h"
#include "Stat/Stat.h"
#include "Interface/IstatHolder.h"
#include <functional>

using namespace Wanted;

class TeamA : public Actor, public IStatHolder
{

    RTTI_DECLARATIONS(TeamA, Actor)

public:
    TeamA();
    virtual ~TeamA();

private:
    virtual void Tick(float deltaTime) override;
    virtual void Draw() override;

    void SpawnUnit();
    void DeleteUnit();

    void Move(const std::vector<Actor*>& selectedObject);
    Vector2 mousePosition;
    Rect dragRect;
    std::vector<Actor*> selectedObject;
};
