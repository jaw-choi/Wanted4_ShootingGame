#pragma once


#include "Actor/Actor.h"

#include "Stat/Stat.h"
#include "Interface/IstatHolder.h"
#include <functional>

using namespace Wanted;

class TeamA : public Actor//, public IStatHolder
{

    RTTI_DECLARATIONS(TeamA, Actor)

public:
    TeamA(const char* image);
    virtual ~TeamA();

    static void Acquire(Level* owner, const char* image, const Vector2& pos);

private:
    virtual void Tick(float deltaTime) override;
    virtual void Draw() override;

    void DeleteUnit();

    void Move(const std::vector<Actor*>& selectedObject);

};
