#include "TeamSpawner.h"
#include "TeamA.h"
#include "TeamB.h"
#include "Core/Input.h"

TeamSpawner::TeamSpawner()
{
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
            mouseCurr = Input::Get().MousePosition();

            dragRect.width = mouseCurr.x - dragStart.x;
            dragRect.height = mouseCurr.y - dragStart.y;

            isDragging = false;
        }



        // 이렇게 하면 위 아래 할 때 달라지지 않으려나? - 일 단 구현 후 수정 하기
        //Enemy::Acquire(GetOwner(), image, Input::Get().MousePosition());
        //DrawDragRect(dragRect);

        if (Input::Get().GetKey(VK_RBUTTON))
        {
            if (!selectedObject.empty())
            {
                // move selected object to current position with A* Algorithm.
            }
        }

        // Todo: 나중에 teamB가 주변에 있으면 공격하는 기능 추가
}



void TeamSpawner::Draw()
{
    super::Draw();
    //Renderer::Get().SubmitFromFile(this->image, position, color, sortingOrder);
    if (isDragging)
        DrawDragRect(dragRect);
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
            Renderer::Get().Submit("+", Vector2(i, j), color, sorting);
        }
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
