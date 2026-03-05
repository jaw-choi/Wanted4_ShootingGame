#include "Core/Input.h"    
#include "TeamA.h"

TeamA::TeamA()
{
}

TeamA::~TeamA()
{
}

void TeamA::Tick(float deltaTime)
{
    //if (deltaTime <= 0.0f)
    //{
    //    return;
    //}
    //super::Tick(deltaTime);

    //if (Input::Get().GetKeyDown(VK_ESCAPE))
    //{
    //    QuitGame();
    //}

    //// 종료 처리.
    //if (Input::Get().GetKeyDown('a'))
    //{
    //    SpawnUnit();
    //}

    //if (Input::Get().GetKeyDown(VK_LBUTTON))
    //{
    //    mousePosition = Input::Get().MousePosition();
    //    dragRect.x = mousePosition.x;
    //    dragRect.y = mousePosition.y;
    //}
    //if (Input::Get().GetKeyUp(VK_LBUTTON))
    //{
    //    // 기존 것 삭제
    //    selectedObject.clear();

    //    int startX = mousePosition.x;
    //    int startY = mousePosition.y;
    //    int endX = Input::Get().MousePosition().x;
    //    int endY = Input::Get().MousePosition().y;
    //    dragRect.width = endX - startX;
    //    dragRect.height = endY - startY;
    //    if (dragRect.Contains(this))
    //    {
    //        selectedObject.emplace_back(this);
    //    }

    //    //Rect 내의 TeamA Actor를 선택함
    //}
    //if (Input::Get().GetKey(VK_RBUTTON))
    //{
    //    if (!selectedObject.empty())
    //    {
    //        // move selected object to current position with A* Algorithm.
    //    }
    //}

    //// Todo: 나중에 teamB가 주변에 있으면 공격하는 기능 추가

    
}

void TeamA::Draw()
{
}

void TeamA::SpawnUnit()
{
}

void TeamA::DeleteUnit()
{
}

void TeamA::Move(const std::vector<Actor*>& selectedObject)
{
}
