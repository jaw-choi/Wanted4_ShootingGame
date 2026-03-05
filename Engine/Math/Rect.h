#pragma once

#include "Vector2.h"
#include "Common/Common.h"
#include "Actor/Actor.h"

namespace Wanted
{
    struct WANTED_API Rect
    {
        int x, y, width, height;
        Rect() : x(0), y(0), width(0), height(0) { }
        Rect(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) { }

        bool Contains(const Actor* actor) const;
        bool Intersects(const Rect& bounds) const;
        bool ContainsRect(const Rect& r) const;
    };
}