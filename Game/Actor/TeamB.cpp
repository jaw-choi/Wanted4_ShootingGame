#include "Core/Input.h"    
#include "TeamB.h"
#include "../Actor/Enemy.h"
#include "Level/Level.h"

TeamB::TeamB(const char* image) : super(image)
{
    SetCollisionLayer(Actor::Layer_TeamB);
    SetCollisionMask(Actor::Layer_All & ~Actor::Layer_Map);
    sortingOrder = 10;
}

TeamB::~TeamB()
{
}

void TeamB::Tick(float deltaTime)
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


    // Todo: 나중에 teamB가 주변에 있으면 공격하는 기능 추가


}

void TeamB::Draw()
{
    Renderer::Get().SubmitFromFile(this->image, position, color, sortingOrder);
}

void TeamB::Acquire(Level* owner, const char* image, const Vector2& pos)
{
    TeamB* object = new TeamB(image);
    object->SetPosition(pos);
    if (owner && object->GetOwner() == nullptr)
    {
        owner->AddNewActor(object);
    }

}

void TeamB::DeleteUnit()
{
}

void TeamB::Move(const std::vector<Actor*>& selectedObject)
{
}
