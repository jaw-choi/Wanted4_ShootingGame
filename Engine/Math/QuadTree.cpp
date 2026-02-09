#include "QuadTree.h"
#include "Util/Util.h"
#include "Actor/Actor.h"

namespace Wanted
{
    QuadTree::QuadTree(const Rect& bounds, int capacity, int depth, int maxDepth)
        :bounds(bounds), capacity(capacity), depth(depth), maxDepth(maxDepth) 
    {
        for (int i = 0; i < 4; i++)
            nodes[i] = nullptr;
    }
    QuadTree::~QuadTree()
    {
        for (int i = 0; i < 4; i++)
            SafeDelete(nodes[i]);
    }
    void QuadTree::Clear()
    {
        objects.clear();
        for (int i = 0; i < 4; i++)
            SafeDelete(nodes[i]);
    }
    void QuadTree::Query(std::vector<Actor*>& returnObjects, const Actor* actor) const
    {
        // Todo: returnObjets 자료구조를 set등으로 해서 중복 없애기
        // 디버그 해보고 수 많으면 실행
        
        // actor의 bounds 생성
        Rect actorBounds(actor->GetPosition().x, actor->GetPosition().y,
            actor->GetWidth(), actor->GetHeight());
        // 현재 노드의 자식 노드가 존재할 때, 겹치는 부분이 있다면
        if (HasNodes())
        {
            for (int i = 0; i < 4; i++)
            {
                if (nodes[i]->bounds.Intersects(actorBounds))
                {
                    nodes[i]->Query(returnObjects, actor);
                }
            }
        }

        // 현재 노드에 저장된 객체들을 returnObjects에 추가.
        for (Actor* obj : objects)
        {
            if (obj == actor)
                continue;

            //returnObjects.emplace_back(obj);

            // 범위와 겹치는 후보만 추가
            Rect objBounds(obj->GetPosition().x, obj->GetPosition().y,
                obj->GetWidth(), obj->GetHeight());

            if (objBounds.Intersects(actorBounds))
                returnObjects.emplace_back(obj);
        }
    }
    void QuadTree::DebugCollectBounds(std::vector<std::string>& out, int maxDepthToPrint, int onlyDepth) const
    {
        DebugCollectBoundsRecursive(out, maxDepthToPrint, onlyDepth);
    }

    void QuadTree::DebugCollectRects(std::vector<DebugRect>& out, int maxDepthToPrint, int onlyDepth) const
    {
        DebugCollectRectsRecursive(out, maxDepthToPrint, onlyDepth);
    }

    void QuadTree::DebugCollectBoundsRecursive(std::vector<std::string>& out, int maxDepthToPrint, int onlyDepth) const
    {
        const bool withinMaxDepth = (maxDepthToPrint < 0) || (depth <= maxDepthToPrint);
        const bool matchesOnlyDepth = (onlyDepth < 0) || (depth == onlyDepth);

        if (withinMaxDepth && matchesOnlyDepth)
        {
            char buffer[128] = {};
            sprintf_s(
                buffer,
                128,
                "[d=%d] x=%d y=%d w=%d h=%d objs=%zu",
                depth,
                bounds.x,
                bounds.y,
                bounds.width,
                bounds.height,
                objects.size()
            );
            out.emplace_back(buffer);
        }

        if (!HasNodes())
        {
            return;
        }

        if (maxDepthToPrint >= 0 && depth >= maxDepthToPrint)
        {
            return;
        }

        if (onlyDepth >= 0 && depth > onlyDepth)
        {
            return;
        }

        for (int i = 0; i < 4; i++)
        {
            if (nodes[i])
            {
                nodes[i]->DebugCollectBoundsRecursive(out, maxDepthToPrint, onlyDepth);
            }
        }
    }

    void QuadTree::DebugCollectRectsRecursive(std::vector<DebugRect>& out, int maxDepthToPrint, int onlyDepth) const
    {
        const bool withinMaxDepth = (maxDepthToPrint < 0) || (depth <= maxDepthToPrint);
        const bool matchesOnlyDepth = (onlyDepth < 0) || (depth == onlyDepth);

        if (withinMaxDepth && matchesOnlyDepth)
        {
            DebugRect entry;
            entry.bounds = bounds;
            entry.depth = depth;
            entry.objectCount = objects.size();
            out.emplace_back(entry);
        }

        if (!HasNodes())
        {
            return;
        }

        if (maxDepthToPrint >= 0 && depth >= maxDepthToPrint)
        {
            return;
        }

        if (onlyDepth >= 0 && depth > onlyDepth)
        {
            return;
        }

        for (int i = 0; i < 4; i++)
        {
            if (nodes[i])
            {
                nodes[i]->DebugCollectRectsRecursive(out, maxDepthToPrint, onlyDepth);
            }
        }
    }
    void QuadTree::Split()
    {
        // 더 이상 쪼갤 의미 없음
        if (bounds.width <= 1 || bounds.height <= 1)
            return; 

        int midX = bounds.width  / 2;
        int midY = bounds.height / 2;
        int x = bounds.x;
        int y = bounds.y;

        nodes[0] = new QuadTree(Rect(x, y, midX, midY), capacity, depth + 1, maxDepth);
        nodes[1] = new QuadTree(Rect(x + midX, y, midX, midY), capacity, depth + 1, maxDepth);
        nodes[2] = new QuadTree(Rect(x, y + midY, midX, midY), capacity, depth + 1, maxDepth);
        nodes[3] = new QuadTree(Rect(x + midX, y + midY, bounds.width - midX, bounds.height - midY), capacity, depth + 1, maxDepth);
        //nodes[3] = new QuadTree(Rect(x + midX, y + midY, midX, midY), capacity, depth + 1, maxDepth);
    }
    void QuadTree::Insert(Actor* actor)
    {
        if (!actor) return;

        if (HasNodes())
        {
            const int idx = GetIndex(actor);
            if (idx != -1 && nodes[idx])
            {
                nodes[idx]->Insert(actor);
                return;
            }
        }

        objects.emplace_back(actor);

        if (objects.size() > static_cast<size_t>(capacity) && depth < maxDepth)
        {
            if (!HasNodes())
                Split();

            size_t i = 0;
            while (i < objects.size())
            {
                Actor* a = objects[i];
                const int idx = GetIndex(a);

                if (idx != -1 && nodes[idx])
                {
                    nodes[idx]->Insert(a);
                    objects[i] = objects.back();
                    objects.pop_back();
                    // i 증가 없음 (swap된 새 원소 재검사)
                }
                else
                {
                    ++i;
                }
            }
        }
    }

    int QuadTree::GetIndex(const Actor* actor) const
    {
        if (HasNodes())
        {
            Rect actorBounds(actor->GetPosition().x, actor->GetPosition().y,
                actor->GetWidth(), actor->GetHeight());
            for (int i = 0; i < 4; i++)
            {
                if (nodes[i]->bounds.ContainsRect(actorBounds))
                    return i;
            }
        }
        return -1;

    }
}
