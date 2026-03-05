#include "Obstacle.h"

Obstacle::Obstacle(const Vector2& position)
    : super("#", position, Color::White)
{
    sortingOrder = 0;
}
