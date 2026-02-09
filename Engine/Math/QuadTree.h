#pragma once

#include <vector>
#include <string>
#include <cstddef>
#include "Common/Common.h"
#include "Rect.h"
#include "Actor/Actor.h"

//class Actor;

namespace Wanted
{
    class WANTED_API QuadTree
    {
    public:
        struct DebugRect
        {
            DebugRect()
                : bounds(0, 0, 0, 0)
            {
            }
            Rect bounds;
            int depth = 0;
            size_t objectCount = 0;
        };

        QuadTree(const Rect& bounds, int capacity, int depth, int maxDepth);
        ~QuadTree();

        // --- 복사 및 대입 금지 ---
        // Rule of Three
        // 복사를 허용하지 않을 것이므로 명시적으로 금지
        // 이를 통해 컴파일러가 실수로 얕은 복사를 하는 것을 막음
        QuadTree(const QuadTree&) = delete;
        QuadTree& operator=(const QuadTree&) = delete;

        // 자식 노드들 (4개 : 북서, 북동, 남서, 남동).
        QuadTree* nodes[4];

        // 자식 노드를 생성하고 현재 노드의 객체들을 분배
        void Split();

        // 트리에 객체 삽입
        void Insert(Actor* actor);

        //특정 객체가 몇번째 인덱스의 자식 노드에 속하는지 
        int GetIndex(const Actor* actor) const;

        // 트리를 비움
        void Clear();

        // 오브젝트를 받아서 특정 객체와 충돌 할 가능성이 있는 객체들만 반환
        void Query(std::vector<Actor*>& returnObjects, const Actor* actor) const;

        // 디버그: 노드 bounds 문자열 수집
        void DebugCollectBounds(std::vector<std::string>& out, int maxDepthToPrint = -1, int onlyDepth = -1) const;

        // 디버그: 노드 bounds 사각형 수집
        void DebugCollectRects(std::vector<DebugRect>& out, int maxDepthToPrint = -1, int onlyDepth = -1) const;

    private:
        inline bool HasNodes() const { return nodes[0] != nullptr; }
        void DebugCollectBoundsRecursive(std::vector<std::string>& out, int maxDepthToPrint, int onlyDepth) const;
        void DebugCollectRectsRecursive(std::vector<DebugRect>& out, int maxDepthToPrint, int onlyDepth) const;

        Rect bounds;
        int capacity;
        int depth;
        int maxDepth;

        std::vector<Actor*> objects;
    };
}

