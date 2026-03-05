#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

class Obstacle : public Actor
{
    RTTI_DECLARATIONS(Obstacle, Actor)

public:
    // 위치 지정이 가능하도록 생성자 선언.
    Obstacle(const Vector2& position);
};
