#pragma once
#include "Actor/Actor.h"
#include "Math/Rect.h"

using namespace Wanted;

class TeamSpawner : public Actor
{
        RTTI_DECLARATIONS(TeamSpawner, Actor)
public:
    TeamSpawner();

private:
    virtual void Tick(float deltaTime) override;
    virtual void Draw() override;

    void SpawnUnitA(Level* owner, const Vector2& pos);
    void SpawnUnitB(Level* owner, const Vector2& pos);
private:
    void DrawDragRect(const Rect& rect);
    bool isDragging = false;
    Vector2 dragStart;
    Vector2 mouseCurr;
    Rect dragRect;
    std::vector<Actor*> selectedObject;
};

