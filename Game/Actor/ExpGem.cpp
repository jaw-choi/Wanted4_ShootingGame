#include "ExpGem.h"
#include "Player.h"
#include "Level/Level.h"

#include <crtdbg.h>
#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif

std::vector<ExpGem*> ExpGem::pool;


ExpGem::ExpGem(const Vector2& position) :super("+", position, Color::Red)
{
    sortingOrder = 1;
}

ExpGem::~ExpGem()
{
}

ExpGem* ExpGem::Acquire(Level* owner, const Vector2& position)
{
    ExpGem* gem = nullptr;
    if (!pool.empty())
    {
        gem = pool.back();
        pool.pop_back();
        gem->inPool = false;
        gem->Initialize(position);
    }
    else
    {
        gem = new ExpGem(position);
    }

    if (owner && gem->GetOwner() == nullptr)
    {
        owner->AddNewActor(gem);
    }

    return gem;
}

void ExpGem::Prewarm(Level* owner, int count)
{
    if (count <= 0)
    {
        return;
    }

    for (int i = 0; i < count; ++i)
    {
        ExpGem* gem = new ExpGem(Vector2::Zero);
        gem->inPool = true;
        gem->isActive = false;
        gem->destroyRequested = false;
        pool.emplace_back(gem);

        if (owner)
        {
            owner->AddNewActor(gem);
        }
    }
}

void ExpGem::ReleaseToPool()
{
    if (inPool)
    {
        return;
    }

    inPool = true;
    isActive = false;
    destroyRequested = false;

    pool.emplace_back(this);
}

void ExpGem::Initialize(const Vector2& position)
{
    isActive = true;
    destroyRequested = false;

    SetPosition(position);
    sortingOrder = 1;
}


void ExpGem::Tick(float deltaTime)
{
    super::Tick(deltaTime);
}


