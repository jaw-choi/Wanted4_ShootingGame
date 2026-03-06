#include "Core/Input.h"    
#include "TeamA.h"
#include "../Actor/Enemy.h"
#include "Level/Level.h"
#include <iostream>

TeamA::TeamA(const char* image) : super(image)
{
}

TeamA::~TeamA()
{
}

void TeamA::Tick(float deltaTime)
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

}

void TeamA::Draw()
{
    //super::Draw();
    Renderer::Get().SubmitFromFile(this->image, position, color, sortingOrder);
}

void TeamA::Acquire(Level* owner, const char* image, const Vector2& pos)
{
    TeamA* object = new TeamA(image);
    object->SetPosition(pos);
    if (owner && object->GetOwner() == nullptr)
    {
        owner->AddNewActor(object);
    }

}

void TeamA::DeleteUnit()
{
}

void TeamA::Move(const std::vector<Actor*>& selectedObject)
{
}
