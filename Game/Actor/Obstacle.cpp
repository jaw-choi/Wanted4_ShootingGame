#include "Obstacle.h"

Obstacle::Obstacle(const Vector2& position)
    : super("#", position, Color::White)
{
    SetCollisionLayer(Actor::Layer_Map);
    SetCollisionMask(Actor::Layer_All & ~Actor::Layer_TeamA & ~Actor::Layer_TeamB);
    sortingOrder = 0;
}
