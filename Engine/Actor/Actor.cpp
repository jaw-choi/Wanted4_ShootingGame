#include "Actor.h"
#include "Util/Util.h"
#include "Render/Renderer.h"
#include "Engine/Engine.h"

#include <iostream>
#include <Windows.h>

#include <crtdbg.h>
#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif

namespace Wanted
{
    Actor::Actor(
        const char* image,
        const Vector2& position,
        Color color)
        : position(position), color(color)
    {
        // 문자열 복사.
        height = 1;
        width = static_cast<int>(strlen(image));
        this->image = new char[width + 1];
        strcpy_s(this->image, width + 1, image);
    }

    Actor::~Actor()
    {
        // 메모리 해제.
        SafeDeleteArray(image);
    }

    void Actor::BeginPlay()
    {
        // 이벤트를 받은 후에는 플래그 설정.
        hasBeganPlay = true;
    }

    void Actor::Tick(float deltaTime)
    {

    }

    void Actor::Draw()
    {
        //Renderer::Draw(position, color, image);

        // 렌더러에 데이터 제출.
        Renderer::Get().Submit(image, position, color, sortingOrder);
        //Renderer::Get().SubmitWorld(image, position, color, sortingOrder);
        //Renderer::Get().SubmitFromFile(image, position, color, sortingOrder);
    }

    void Actor::Destroy()
    {
        // 삭제 플래그 설정.
        destroyRequested = true;

        // 삭제 이벤트 호출.
        OnDestroy();
    }

    void Actor::OnDestroy()
    {
    }

    void Actor::QuitGame()
    {
        // 엔진 종료 요청.
        Engine::Get().QuitEngine();
    }

    bool Actor::TestIntersect(const Actor* const other)
    {
        // AABB (Axis Aligned Bounding Box)
        // padding 포함 + 최소 1x1 보장

        // --- 패딩 값 ---
        const int paddingX = 1;
        const int paddingY = 1;

        // --- 내 AABB ---
        int xMin = position.x + paddingX;
        int xMax = position.x + width - 1 - paddingX;

        int yMin = position.y + paddingY;
        int yMax = position.y + height - 1 - paddingY;

        // 최소 1칸 보장 (역전 방지)
        if (xMax < xMin) xMax = xMin;
        if (yMax < yMin) yMax = yMin;

        // --- 상대 AABB ---
        int otherXMin = other->position.x + paddingX;
        int otherXMax = other->position.x + other->width - 1 - paddingX;

        int otherYMin = other->position.y + paddingY;
        int otherYMax = other->position.y + other->height - 1 - paddingY;

        // 최소 1칸 보장 (역전 방지)
        if (otherXMax < otherXMin) otherXMax = otherXMin;
        if (otherYMax < otherYMin) otherYMax = otherYMin;

        // --- 분리 조건 (Separating Axis Theorem) ---

        // x축 분리
        if (otherXMin > xMax) return false;
        if (otherXMax < xMin) return false;

        // y축 분리
        if (otherYMin > yMax) return false;
        if (otherYMax < yMin) return false;

        // x, y 모두 겹침
        return true;
    }


    void Actor::ChangeImage(const char* newImage)
    {
        // 기존 메모리 해제.
        SafeDeleteArray(image);

        // 새로운 문자열 복사.
        width = static_cast<int>(strlen(newImage));
        image = new char[width + 1];
        strcpy_s(image, width + 1, newImage);
    }

    void Actor::SetPosition(const Vector2& newPosition)
    {
        // 렌더러에 빈칸 그리기 요청.
        //Renderer::Draw(position, ' ');

        // 변경하려는 위치가 현재 위치와 같으면 건너뀜.
        if (position == newPosition)
        {
            return;
        }

        // 새로운 위치 설정.
        position = newPosition;
    }
}

