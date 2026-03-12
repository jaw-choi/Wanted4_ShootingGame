#include "TeamSpawner.h"
#include "TeamA.h"
#include "TeamB.h"
#include "Core/Input.h"
#include "Level/GameLevel.h"
#include <iostream>
#include <cmath>
#include <queue>
#include <limits>
#include <algorithm>
#include <fstream>
#include <string>


TeamSpawner::TeamSpawner()
{
    
}

static constexpr int kMaxReplanAttempts = 3;
static constexpr int kMaxPathRequestsPerFrame = 3;
static constexpr float kReplanCooldownSeconds = 0.15f;
static constexpr int kTrailStep = 3;
static constexpr int kTrailLength = 40;
static constexpr float kTrailAnimInterval = 0.08f;

static bool AabbOverlap(const Vector2& aPos, int aW, int aH, const Vector2& bPos, int bW, int bH)
{
    if (aW <= 0 || aH <= 0 || bW <= 0 || bH <= 0)
    {
        return false;
    }

    const int aRight = aPos.x + aW;
    const int aBottom = aPos.y + aH;
    const int bRight = bPos.x + bW;
    const int bBottom = bPos.y + bH;

    if (aRight <= bPos.x || bRight <= aPos.x || aBottom <= bPos.y || bBottom <= aPos.y)
    {
        return false;
    }

    return true;
}

static Rect NormalizeRect(const Rect& rect)
{
    Rect normalized = rect;
    if (normalized.width < 0)
    {
        normalized.x += normalized.width;
        normalized.width = -normalized.width;
    }

    if (normalized.height < 0)
    {
        normalized.y += normalized.height;
        normalized.height = -normalized.height;
    }

    return normalized;
}

static bool IsControllableUnit(const Actor* actor)
{
    return actor && (actor->As<TeamA>() != nullptr || actor->As<TeamB>() != nullptr);
}

static bool IsBlockedByOtherUnit(const GameLevel* level, const Actor* self,
    const Vector2& pos, int width, int height)
{
    if (!level)
    {
        return false;
    }

    for (Actor* other : level->GetActors())
    {
        if (!other || other == self || !other->IsActive())
        {
            continue;
        }

        if (!IsControllableUnit(other))
        {
            continue;
        }

        if (AabbOverlap(pos, width, height,
            other->GetPosition(), other->GetWidth(), other->GetHeight()))
        {
            return true;
        }
    }
    

    return false;
}

static bool GetAsciiSizeFromFile(const char* path, int& outW, int& outH)
{
    outW = 0;
    outH = 0;

    if (!path)
    {
        return false;
    }

    std::ifstream in(path);
    if (!in.is_open())
    {
        return false;
    }

    std::string line;
    if (!std::getline(in, line))
    {
        return false;
    }

    if (!line.empty() && line.back() == '\r')
    {
        line.pop_back();
    }

    outW = static_cast<int>(line.size());
    outH = 1;

    while (std::getline(in, line))
    {
        outH++;
    }

    return outW > 0 && outH > 0;
}

static bool CanPlaceUnitAt(const GameLevel* level, const Vector2& pos, const char* imagePath)
{
    if (!level || !imagePath)
    {
        return false;
    }

    int width = 0;
    int height = 0;
    if (!GetAsciiSizeFromFile(imagePath, width, height))
    {
        return false;
    }

    if (level->IsBlockedByMap(pos, width, height))
    {
        return false;
    }

    if (IsBlockedByOtherUnit(level, nullptr, pos, width, height))
    {
        return false;
    }

    return true;
}

static std::vector<std::pair<int,int>> closedPath;

static bool IsBlockedByUnitGrid(
    const std::vector<const Actor*>& unitGrid,
    int gridW,
    int gridH,
    const Actor* self,
    const Vector2& pos,
    int width,
    int height)
{
    if (unitGrid.empty() || gridW <= 0 || gridH <= 0 || width <= 0 || height <= 0)
    {
        return false;
    }

    const int left =   max(0, pos.x);
    const int top =    max(0, pos.y);
    const int right =  min(gridW, pos.x + width);
    const int bottom = min(gridH, pos.y + height);

    if (right <= left || bottom <= top)
    {
        return false;
    }

    for (int y = top; y < bottom; ++y)
    {
        const int row = y * gridW;
        for (int x = left; x < right; ++x)
        {
            const Actor* occupant = unitGrid[row + x];
            if (occupant && occupant != self)
            {
                return true;
            }
        }
    }

    return false;
}

static std::vector<Vector2> FindPathAStar(
    const GameLevel* level,
    const Actor* self,
    const Vector2& start,
    const Vector2& goal,
    int actorWidth,
    int actorHeight,
    bool sizeOne,
    const std::vector<const Actor*>* unitGrid,
    int gridW,
    int gridH)
{
    std::vector<Vector2> empty;
    if (!level || actorWidth <= 0 || actorHeight <= 0)
    {
        return empty;
    }

    const int mapW = level->GetMapWidth();
    const int mapH = level->GetMapHeight();
    if (mapW <= 0 || mapH <= 0)
    {
        return empty;
    }

    auto Clamp = [](int v, int lo, int hi)
    {
        if (v < lo) return lo;
        if (v > hi) return hi;
        return v;
    };

    const int startX = Clamp(start.x, 0, mapW - 1);
    const int startY = Clamp(start.y, 0, mapH - 1);
    const int goalX = Clamp(goal.x, 0, mapW - 1);
    const int goalY = Clamp(goal.y, 0, mapH - 1);

    const int startIndex = startY * mapW + startX;
    const int goalIndex = goalY * mapW + goalX;

    if (sizeOne)
    {
        closedPath.clear();
    }

    const float INF = 10000.f;
    const int total = mapW * mapH;

    std::vector<float> gScore(total, INF);
    std::vector<int> cameFrom(total, -1);
    std::vector<bool> closed(total, false);

    auto Heuristic = [goalX, goalY](int x, int y) -> float
    {
        return static_cast<float>(std::abs(goalX - x) + std::abs(goalY - y));
    };

    auto IsBlocked = [&](int x, int y) -> bool
    {
        if (x < 0 || y < 0 || x >= mapW || y >= mapH)
        {
            return true;
        }
        const Vector2 pos(x, y);
        if (level->IsBlockedByMap(pos, actorWidth, actorHeight))
        {
            return true;
        }
        if (unitGrid && !unitGrid->empty())
        {
            if (IsBlockedByUnitGrid(*unitGrid, gridW, gridH, self, pos, actorWidth, actorHeight))
            {
                return true;
            }
        }
        else if (IsBlockedByOtherUnit(level, self, pos, actorWidth, actorHeight))
        {
            return true;
        }
        return false;
    };

    if (IsBlocked(goalX, goalY))
    {
        return empty;
    }

    struct Node
    {
        int index;
        float f;
    };

    struct NodeGreater
    {
        bool operator()(const Node& a, const Node& b) const { return a.f > b.f; }
    };

    std::priority_queue<Node, std::vector<Node>, NodeGreater> open;
    gScore[startIndex] = 0.0f;
    open.push({ startIndex, Heuristic(startX, startY) });

    bool reachedGoal = (startIndex == goalIndex);

    const int dirs[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };

    while (!open.empty())
    {
        Node current = open.top();
        open.pop();

        if (closed[current.index])
        {
            continue;
        }

        closed[current.index] = true;

        const int cx = current.index % mapW;
        const int cy = current.index / mapW;
        if (sizeOne)
            closedPath.push_back(std::make_pair(cx, cy));

        if (current.index == goalIndex)
        {
            reachedGoal = true;
            break;
        }

        for (int i = 0; i < 4; ++i)
        {
            const int nx = cx + dirs[i][0];
            const int ny = cy + dirs[i][1];

            if (IsBlocked(nx, ny))
            {
                continue;
            }

            const int nIndex = ny * mapW + nx;
            if (closed[nIndex])
            {
                continue;
            }

            const float tentativeG = gScore[current.index] + 1.0f;
            if (tentativeG < gScore[nIndex])
            {
                cameFrom[nIndex] = current.index;
                gScore[nIndex] = tentativeG;
                const float f = tentativeG + Heuristic(nx, ny);
                open.push({ nIndex, f });
            }
        }
    }

    if (!reachedGoal)
    {
        if (sizeOne)
        {
            closedPath.clear();
        }
        return empty;
    }

    std::vector<Vector2> path;
    int trace = goalIndex;
    if (trace == -1)
    {
        return empty;
    }

    while (trace != -1)
    {
        const int x = trace % mapW;
        const int y = trace / mapW;
        path.emplace_back(x, y);
        trace = cameFrom[trace];
    }

    std::reverse(path.begin(), path.end());
    return path;
}


void TeamSpawner::Tick(float deltaTime)
{

        if (deltaTime <= 0.0f)
        {
            return;
        }
        super::Tick(deltaTime);
        trailAnimTime += deltaTime;

        //if (Input::Get().GetKeyDown(VK_ESCAPE))
        //{
        //    QuitGame();

        //}

        if (Input::Get().GetKeyDown('A') || Input::Get().GetKeyDown('a'))
        {
            SpawnUnitA(GetOwner(),Input::Get().MousePosition());
            int x = Input::Get().MousePosition().x;
            int y = Input::Get().MousePosition().y;

        }

        if (Input::Get().GetKeyDown('B') || Input::Get().GetKeyDown('b'))
        {
            SpawnUnitB(GetOwner(), Input::Get().MousePosition());
            int x = Input::Get().MousePosition().x;
            int y = Input::Get().MousePosition().y;

        }

        if (Input::Get().GetKeyDown(VK_LBUTTON))
        {
            dragStart = Input::Get().MousePosition();
            mouseCurr = dragStart;
            isDragging = true;

            dragRect.x = dragStart.x;
            dragRect.y = dragStart.y;
            dragRect.width = 0;
            dragRect.height = 0;
        }

        if (isDragging && Input::Get().GetKey(VK_LBUTTON))
        {
            mouseCurr = Input::Get().MousePosition();

            dragRect.width = mouseCurr.x - dragStart.x;
            dragRect.height = mouseCurr.y - dragStart.y;
        }
        if (isDragging && Input::Get().GetKeyUp(VK_LBUTTON))
        {
            for (const auto& actor : selectedObject)
            {
                actor->SetColor(Color::White);
            }
            selectedObject.clear();
            mouseCurr = Input::Get().MousePosition();

            dragRect.width = mouseCurr.x - dragStart.x;
            dragRect.height = mouseCurr.y - dragStart.y;

            isDragging = false;
            const Rect normalizedRect = NormalizeRect(dragRect);
            
            for (const auto& actor : static_cast<GameLevel*>(GetOwner())->GetActors())
            {
                if (!actor || !actor->IsActive())
                {
                    continue;
                }

                if (!IsControllableUnit(actor))
                {
                    continue;
                }

                if (normalizedRect.Contains(actor))
                {
                    selectedObject.emplace_back(actor);
                }                
            }
            for (const auto& actor : selectedObject)
            {
                if(actor->IsTypeOf<TeamA>())
                    actor->SetColor(Color::Blue);
                else if(actor->IsTypeOf<TeamB>())
                    actor->SetColor(Color::Red);

            }
            //for(const auto actor : )
            //if(dragRect.Contains())
            //    dragRect.con
            //selectedObject.emplace_back();
        }



        // 이렇게 하면 위 아래 할 때 달라지지 않으려나? - 일 단 구현 후 수정 하기
        //Enemy::Acquire(GetOwner(), image, Input::Get().MousePosition());
        //DrawDragRect(dragRect);

        // 우측 마우스 클릭
        if (Input::Get().GetMouseButtonDown(1))
        {
            if (!selectedObject.empty())
            {
                    StartMoveSelected(Input::Get().MousePosition());
            }
        }

        // Todo: 나중에 teamB가 주변에 있으면 공격하는 기능 추가

        ProcessPathRequests();
        UpdateMoveSelected(deltaTime);
}



void TeamSpawner::Draw()
{
    super::Draw();
    //Renderer::Get().SubmitFromFile(this->image, position, color, sortingOrder);
    if (isDragging)
        DrawDragRect(dragRect);

    DrawMoveDebug();
    for (const auto& path : closedPath)
    {
        Renderer::Get().Submit("+", Vector2(path.first, path.second), Color::Magenta, 1);
    }
}


void TeamSpawner::DrawDragRect(const Rect& rect)
{

    Color color = Color::White;
    const Rect normalized = NormalizeRect(rect);

    const int sorting = 0;
    const int leftX = normalized.x;
    const int rightX = normalized.x + normalized.width;
    const int topY = normalized.y;
    const int bottomY = normalized.y + normalized.height;

    for (int i = leftX; i < rightX; i++)
    {
        for (int j = topY; j < bottomY; j++)
        {
            if (i == leftX || i == rightX - 1 || j == topY || j == bottomY - 1)
                Renderer::Get().Submit("+", Vector2(i, j), color, sorting);
        }
    }

}

void TeamSpawner::DrawMoveDebug() const
{
    if (movePaths.empty())
    {
        return;
    }

    const int phase = (kTrailStep > 0)
        ? static_cast<int>(trailAnimTime / kTrailAnimInterval) % kTrailStep
        : 0;

    for (const auto& entry : movePaths)
    {
        Actor* actor = entry.first;
        if (!actor || !actor->IsActive())
        {
            continue;
        }

        const MovePath& path = entry.second;
        if (path.nodes.empty())
        {
            continue;
        }

        if (path.index >= path.nodes.size())
        {
            continue;
        }

        const size_t start = path.index + static_cast<size_t>(phase);
        const size_t end = min(path.nodes.size(), start + static_cast<size_t>(kTrailLength));

        for (size_t i = start; i < end; i += static_cast<size_t>(kTrailStep))
        {
            Renderer::Get().SubmitFromFile("../Assets/move.txt", path.nodes[i], Color::Yellow, 2);
        }
    }
}

void TeamSpawner::StartMoveSelected(const Vector2& target)
{
    if (selectedObject.empty())
    {
        return;
    }

    GameLevel* gameLevel = dynamic_cast<GameLevel*>(GetOwner());
    if (!gameLevel)
    {
        return;
    }

    pendingPathRequests.clear();
    bool hasTrackedActor = false;

    for (Actor*& actor : selectedObject)
    {
        if (!actor || !actor->IsActive() || !IsControllableUnit(actor))
        {
            continue;
        }

        hasTrackedActor = true;
        movePositions[actor] = Vector2f(actor->GetPosition());

        MovePath& path = movePaths[actor];
        path.target = target;
        path.replanAttempts = 0;
        path.replanCooldown = 0.0f;
        path.waitingForPath = false;
        if (path.nodes.empty())
        {
            path.index = 0;
        }
        QueuePathRequest(actor, target);
    }

    if (!hasTrackedActor)
    {
        return;
    }

    moveTarget = target;
    isMoveCommand = true;
}

void TeamSpawner::UpdateMoveSelected(float deltaTime)
{
    if (!isMoveCommand || deltaTime <= 0.0f)
    {
        return;
    }

    if (selectedObject.empty())
    {
        isMoveCommand = false;
        return;
    }

    const float step = moveSpeed * deltaTime;
    GameLevel* gameLevel = dynamic_cast<GameLevel*>(GetOwner());
    bool allReached = true;

    for (Actor*& actor : selectedObject)
    {
        if (!actor || !actor->IsActive() || !IsControllableUnit(actor))
        {
            continue;
        }

        auto posIt = movePositions.find(actor);
        auto pathIt = movePaths.find(actor);
        if (posIt == movePositions.end() || pathIt == movePaths.end())
        {
            continue;
        }

        Vector2f& currF = posIt->second;
        MovePath& path = pathIt->second;
        if (path.replanCooldown > 0.0f)
        {
            path.replanCooldown = max(0.0f, path.replanCooldown - deltaTime);
        }

        if (path.target != moveTarget)
        {
            path.target = moveTarget;
            path.replanAttempts = 0;
            path.replanCooldown = 0.0f;
            QueuePathRequest(actor, moveTarget);
        }

        if (path.waitingForPath)
        {
            allReached = false;
        }

        if (path.nodes.empty())
        {
            if (!path.waitingForPath &&
                path.replanAttempts < kMaxReplanAttempts &&
                path.replanCooldown <= 0.0f)
            {
                QueuePathRequest(actor, path.target);
                allReached = false;
            }
            continue;
        }

        while (path.index < path.nodes.size() && path.nodes[path.index] == actor->GetPosition())
        {
            path.index++;
        }

        if (path.index >= path.nodes.size())
        {
            continue;
        }

        allReached = false;

        Vector2 nextNode = path.nodes[path.index];
        if (gameLevel)
        {
            const bool blockedByMap = gameLevel->IsBlockedByMap(nextNode, actor->GetWidth(), actor->GetHeight());
            const bool blockedByUnit = IsBlockedByUnitGrid(actor, nextNode, actor->GetWidth(), actor->GetHeight());

            if (blockedByMap || blockedByUnit)
            {
                allReached = false;
                if (!path.waitingForPath &&
                    path.replanAttempts < kMaxReplanAttempts &&
                    path.replanCooldown <= 0.0f)
                {
                    path.replanCooldown = kReplanCooldownSeconds;
                    path.nodes.clear();
                    path.index = 0;
                    QueuePathRequest(actor, moveTarget);
                }
                continue;
            }
        }

        const Vector2f nextF(nextNode);
        const Vector2f delta = nextF - currF;
        const float distSq = delta.LengthSquared();

        if (distSq <= step * step)
        {
            currF = nextF;
            actor->SetPosition(nextNode);
            path.index++;
        }
        else
        {
            currF = currF + delta.Normalized() * step;
            actor->SetPosition(Vector2(
                static_cast<int>(std::round(currF.x)),
                static_cast<int>(std::round(currF.y))
            ));
        }
    }

    if (allReached)
    {
        isMoveCommand = false;
        movePaths.clear();
        movePositions.clear();
        pendingPathRequests.clear();
        closedPath.clear();
    }
}

void TeamSpawner::ProcessPathRequests()
{
    if (pendingPathRequests.empty())
    {
        return;
    }

    GameLevel* gameLevel = dynamic_cast<GameLevel*>(GetOwner());
    if (!gameLevel)
    {
        pendingPathRequests.clear();
        return;
    }

    BuildUnitOccupancy(gameLevel);
    const bool sizeOne = selectedObject.size() == 1;
    int processedCount = 0;

    while (!pendingPathRequests.empty() && processedCount < kMaxPathRequestsPerFrame)
    {
        PathRequest request = pendingPathRequests.front();
        pendingPathRequests.pop_front();
        processedCount++;

        if (!request.actor || !request.actor->IsActive())
        {
            continue;
        }

        auto pathIt = movePaths.find(request.actor);
        if (pathIt == movePaths.end())
        {
            continue;
        }

        MovePath& path = pathIt->second;
        if (path.requestToken != request.requestToken || path.target != request.target)
        {
            continue;
        }

        path.waitingForPath = false;
        path.replanAttempts++;

        std::vector<Vector2> nodes = FindPathAStar(gameLevel, request.actor, request.actor->GetPosition(),
            request.target, request.actor->GetWidth(), request.actor->GetHeight(),
            sizeOne, &unitOccupancy, unitGridW, unitGridH);

        if (nodes.empty())
        {
            path.nodes.clear();
            path.index = 0;

            if (path.replanAttempts >= kMaxReplanAttempts)
            {
                path.replanCooldown = 0.0f;
            }
            else
            {
                path.replanCooldown = kReplanCooldownSeconds;
            }
            continue;
        }

        path.nodes = std::move(nodes);
        path.index = 0;
        path.replanAttempts = 0;
        path.replanCooldown = 0.0f;
    }
}

void TeamSpawner::QueuePathRequest(Actor* actor, const Vector2& target)
{
    if (!actor || !actor->IsActive())
    {
        return;
    }

    MovePath& path = movePaths[actor];
    if (path.waitingForPath)
    {
        return;
    }

    path.target = target;
    path.waitingForPath = true;
    path.requestToken++;

    PathRequest request;
    request.actor = actor;
    request.target = target;
    request.requestToken = path.requestToken;
    pendingPathRequests.push_back(request);
}

void TeamSpawner::BuildUnitOccupancy(const GameLevel* level)
{
    if (!level)
    {
        unitGridW = 0;
        unitGridH = 0;
        unitOccupancy.clear();
        return;
    }

    unitGridW = level->GetMapWidth();
    unitGridH = level->GetMapHeight();
    if (unitGridW <= 0 || unitGridH <= 0)
    {
        unitGridW = 0;
        unitGridH = 0;
        unitOccupancy.clear();
        return;
    }

    const size_t total = static_cast<size_t>(unitGridW) * static_cast<size_t>(unitGridH);
    if (unitOccupancy.size() != total)
    {
        unitOccupancy.assign(total, nullptr);
    }
    else
    {
        std::fill(unitOccupancy.begin(), unitOccupancy.end(), nullptr);
    }

    for (Actor* other : level->GetActors())
    {
        if (!other || !other->IsActive())
        {
            continue;
        }

        if (!IsControllableUnit(other))
        {
            continue;
        }

        const int ow = other->GetWidth();
        const int oh = other->GetHeight();
        if (ow <= 0 || oh <= 0)
        {
            continue;
        }

        const int left = max(0, other->GetPosition().x);
        const int top = max(0, other->GetPosition().y);
        const int right = min(unitGridW, other->GetPosition().x + ow);
        const int bottom = min(unitGridH, other->GetPosition().y + oh);

        for (int y = top; y < bottom; ++y)
        {
            const int row = y * unitGridW;
            for (int x = left; x < right; ++x)
            {
                unitOccupancy[row + x] = other;
            }
        }
    }
}

bool TeamSpawner::IsBlockedByUnitGrid(const Actor* self, const Vector2& pos, int width, int height) const
{
    return ::IsBlockedByUnitGrid(unitOccupancy, unitGridW, unitGridH, self, pos, width, height);
}


void TeamSpawner::SpawnUnitA(Level* owner, const Vector2& pos)
{
    GameLevel* gameLevel = dynamic_cast<GameLevel*>(owner);
    if (!CanPlaceUnitAt(gameLevel, pos, "../Assets/teamA.txt"))
    {
        return;
    }
    TeamA::Acquire(owner,"../Assets/teamA.txt", pos);
}

void TeamSpawner::SpawnUnitB(Level* owner, const Vector2& pos)
{
    GameLevel* gameLevel = dynamic_cast<GameLevel*>(owner);
    if (!CanPlaceUnitAt(gameLevel, pos, "../Assets/teamB.txt"))
    {
        return;
    }
    TeamB::Acquire(owner, "../Assets/teamB.txt", pos);
}
