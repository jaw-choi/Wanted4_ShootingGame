#include "ExpGem.h"
#include "Player.h"


ExpGem::ExpGem(const Vector2& position) :super("+", position, Color::Red)
{
    sortingOrder = 1;
}

ExpGem::~ExpGem()
{
}


void ExpGem::Tick(float deltaTime)
{
    super::Tick(deltaTime);
}
