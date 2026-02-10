#include "Rect.h"

#include <crtdbg.h>
#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif

namespace Wanted
{
    bool Rect::Contains(const Actor* actor) const
    {
        Rect actorBounds(actor->GetPosition().x, actor->GetPosition().y,
            actor->GetWidth(), actor->GetHeight());

        if (actorBounds.x >= x &&
            actorBounds.y >= y &&
            actorBounds.x + actorBounds.width <= x + width &&
            actorBounds.y + actorBounds.height <= y + height)
        {
            return true;
        }
        return false;
    }

    bool Rect::Intersects(const Rect& bounds) const
    {
        // 완전히 겹치지 않는 경우
        if (bounds.x + bounds.width <= x ||
            bounds.y + bounds.height <= y ||
            bounds.x >= x + width ||
            bounds.y >= y + height)
        {
            return false;
        }
        return true;
    }

    bool Rect::ContainsRect(const Rect& r) const
    {
        return (r.x >= x &&
            r.y >= y &&
            r.x + r.width <= x + width &&
            r.y + r.height <= y + height);
    }

}
