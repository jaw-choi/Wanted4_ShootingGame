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
        // actor가 null이면 실행 안함
        if (!actor)
            return;

        // 자식 노드가 존재할 때.
        if (HasNodes())
        {
            int index = GetIndex(actor);
            if (index != -1)
            {
                nodes[index]->Insert(actor);
                return;
            }
        }

        // 자식 노드가 없거나, 자식 노드에 들어가지 않을때.
        objects.emplace_back(actor);

        // 객체 수가 초과 했을 때 maxDepth가 아니라면 -> 분할.
        if (objects.size() > capacity && depth < maxDepth)
        {
            // 자식 노드가 존재 하면 분할.
            if (!HasNodes())
                Split();

            // 현재 노드의 객체들을 자식 노드로 재분배.
            int i = 0;
            while (i < objects.size())
            {
                int index = GetIndex(objects[i]);
                if (index != -1)
                {
                    nodes[index]->Insert(objects[i]);
                    //objects.erase(objects.begin() + i);
                    nodes[index]->Insert(objects[i]);
                    objects[i] = objects.back();
                    objects.pop_back();
                }
                else
                    i++;
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
        //if (HasNodes())
        //{
        //    for (int i = 0; i < 4; i++)
        //    {
        //        if (bounds.Contains(actor))
        //            return i;
        //    }
        //}
        //return -1;
    }
}