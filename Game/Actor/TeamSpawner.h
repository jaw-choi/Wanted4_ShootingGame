#pragma once
#include "Actor/Actor.h"
#include "Math/Rect.h"
#include <deque>
#include <unordered_map>

using namespace Wanted;

class GameLevel;

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
    struct MovePath
    {
        std::vector<Vector2> nodes;
        size_t index = 0;
        Vector2 target = Vector2::Zero;
        int replanAttempts = 0;
        float replanCooldown = 0.0f;
        bool waitingForPath = false;
        unsigned int requestToken = 0;
    };

    struct PathRequest
    {
        Actor* actor = nullptr;
        Vector2 target = Vector2::Zero;
        unsigned int requestToken = 0;
    };

    void DrawDragRect(const Rect& rect);
    void DrawMoveDebug() const;
    void StartMoveSelected(const Vector2& target);
    void UpdateMoveSelected(float deltaTime);
    void ProcessPathRequests();
    void QueuePathRequest(Actor* actor, const Vector2& target);
    void BuildUnitOccupancy(const GameLevel* level);
    bool IsBlockedByUnitGrid(const Actor* self, const Vector2& pos, int width, int height) const;
    bool isDragging = false;
    Vector2 dragStart;
    Vector2 mouseCurr;
    Rect dragRect;
    std::vector<Actor*> selectedObject;
    bool isMoveCommand = false;
    Vector2 moveTarget;
    float moveSpeed = 800.0f;
    std::unordered_map<Actor*, Vector2f> movePositions;
    std::unordered_map<Actor*, MovePath> movePaths;
    std::deque<PathRequest> pendingPathRequests;
    std::vector<const Actor*> unitOccupancy;
    int unitGridW = 0;
    int unitGridH = 0;
    float trailAnimTime = 0.0f;
    
};

