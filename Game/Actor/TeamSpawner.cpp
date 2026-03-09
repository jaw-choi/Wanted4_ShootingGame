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

TeamSpawner::TeamSpawner()
{
    
}


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

        const bool otherIsUnit = other->IsTypeOf<TeamA>() || other->IsTypeOf<TeamB>();
        if (!otherIsUnit)
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

static std::vector<std::pair<int,int>> closedPath;
static std::vector<Vector2> FindPathAStar(
    const GameLevel* level,
    const Actor* self,
    const Vector2& start,
    const Vector2& goal,
    int actorWidth,
    int actorHeight,
    bool sizeOne)
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
        if (IsBlockedByOtherUnit(level, self, pos, actorWidth, actorHeight))
        {
            return true;
        }
        return false;
    };

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

    int bestIndex = startIndex;
    float bestH = Heuristic(startX, startY);

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
        


        const float h = Heuristic(cx, cy);
        if (h < bestH)
        {
            bestH = h;
            bestIndex = current.index;
        }

        if (current.index == goalIndex)
        {
            bestIndex = goalIndex;
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

    std::vector<Vector2> path;
    int trace = bestIndex;
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

        if (Input::Get().GetKeyDown(VK_ESCAPE))
        {
            QuitGame();
        }

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
            
            for (const auto& actor : static_cast<GameLevel*>(GetOwner())->GetActors())
            {
                if (dragRect.Contains(actor))
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

        UpdateMoveSelected(deltaTime);
}



void TeamSpawner::Draw()
{
    super::Draw();
    //Renderer::Get().SubmitFromFile(this->image, position, color, sortingOrder);
    if (isDragging)
        DrawDragRect(dragRect);

    DrawClosePathDebug();
    DrawMoveDebug();
    for (const auto& path : closedPath)
    {
        Renderer::Get().Submit("+", Vector2(path.first, path.second), Color::Magenta, 1);
    }
}


void TeamSpawner::DrawDragRect(const Rect& rect)
{

    Color color = Color::White;

    const int sorting = 0;
    int leftX, rightX, topY, bottomY;
    if (rect.width < 0)
    {
        leftX = rect.x + rect.width;
        rightX = rect.x;
    }
    else
    {
        leftX = rect.x;
        rightX = rect.x + rect.width;
    }

    if (rect.height < 0)
    {
        topY = rect.y + rect.height;
        bottomY = rect.y;
    }
    else
    {
        topY = rect.y;
        bottomY = rect.y + rect.height;
    }

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

        const Vector2 pos = actor->GetPosition();
        const int g = static_cast<int>(path.index);
        const int h = std::abs(moveTarget.x - pos.x) + std::abs(moveTarget.y - pos.y);

        for (const Vector2& node : path.nodes)
        {
            Renderer::Get().SubmitFromFile("../Assets/move.txt", node, Color::Yellow, 1);
        }
    }
}

void TeamSpawner::DrawClosePathDebug() const
{
    if (movePaths.empty())
    {
        return;
    }

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

        const Vector2 pos = actor->GetPosition();
        const int g = static_cast<int>(path.index);
        const int h = std::abs(moveTarget.x - pos.x) + std::abs(moveTarget.y - pos.y);

        for (const Vector2& node : path.nodes)
        {
            Renderer::Get().SubmitFromFile("../Assets/move.txt", node, Color::Yellow, 2);
        }
    }
}

void TeamSpawner::StartMoveSelected(const Vector2& target)
{
    moveTarget = target;
    isMoveCommand = !selectedObject.empty();
    movePositions.clear();
    movePaths.clear();
    bool sizeOne = selectedObject.size() == 1;
    GameLevel* gameLevel = dynamic_cast<GameLevel*>(GetOwner());
    for (Actor* actor : selectedObject)
    {
        if (!actor || !actor->IsActive())
        {
            continue;
        }
        movePositions[actor] = Vector2f(actor->GetPosition());
        MovePath path;
        path.target = moveTarget;
        path.nodes = FindPathAStar(gameLevel, actor, actor->GetPosition(), moveTarget,
            actor->GetWidth(), actor->GetHeight(), sizeOne);
        path.index = 0;
        movePaths[actor] = path;
    }
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
    bool sizeOne = selectedObject.size() == 1;

    for (Actor* actor : selectedObject)
    {
        if (!actor || !actor->IsActive())
        {
            continue;
        }

        Vector2f& currF = movePositions[actor];
        MovePath& path = movePaths[actor];
        if (path.target != moveTarget || path.nodes.empty())
        {
            path.target = moveTarget;
            path.nodes = FindPathAStar(gameLevel, actor, actor->GetPosition(), moveTarget,
                actor->GetWidth(), actor->GetHeight(),sizeOne);
            path.index = 0;
        }

        if (path.nodes.empty())
        {
            allReached = false;
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
            if (gameLevel->IsBlockedByMap(nextNode, actor->GetWidth(), actor->GetHeight()) ||
                IsBlockedByOtherUnit(gameLevel, actor, nextNode, actor->GetWidth(), actor->GetHeight()))
            {
                path.nodes = FindPathAStar(gameLevel, actor, actor->GetPosition(), moveTarget,
                    actor->GetWidth(), actor->GetHeight(), sizeOne);
                path.index = 0;

                while (path.index < path.nodes.size() && path.nodes[path.index] == actor->GetPosition())
                {
                    path.index++;
                }

                if (path.index >= path.nodes.size())
                {
                    continue;
                }

                nextNode = path.nodes[path.index];
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
        closedPath.clear();
    }
}


void TeamSpawner::SpawnUnitA(Level* owner, const Vector2& pos)
{
    TeamA::Acquire(owner,"../Assets/teamA.txt", pos);
}

void TeamSpawner::SpawnUnitB(Level* owner, const Vector2& pos)
{
    TeamB::Acquire(owner, "../Assets/teamB.txt", pos);
}
