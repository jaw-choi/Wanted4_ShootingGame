#include "Actor.h"
#include "Util/Util.h"
#include "Render/Renderer.h"
#include "Engine/Engine.h"

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>

#include <crtdbg.h>
#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif

namespace Wanted
{
    static bool GetAsciiSizeFromFile(const char* path, int& outW, int& outH)
    {
        outW = 0;
        outH = 0;

        std::ifstream in(path);
        if (!in.is_open()) return false;

        std::string line;

        // 첫 줄 읽기 (width 결정)
        if (!std::getline(in, line)) return false;
        if (!line.empty() && line.back() == '\r') line.pop_back(); // CRLF 보정

        outW = (int)line.size();
        outH = 1;

        // 나머지 줄 수 세기 (height)
        while (std::getline(in, line))
            outH++;

        return true;
    }
    Actor::Actor(
        const char* image,
        const Vector2& position,
        Color color)
        : position(position), color(color)
    {
        if (!GetAsciiSizeFromFile(image, width, height))
        {
            // 실패 시 안전 처리(원하면 예외/로그로 바꿔도 됨)
            width = 0;
            height = 0;
            this->image = nullptr;
            return;
        }

        // 파일 경로 문자열 복사 (이미지 폭이 아니라 경로 길이 기준)
        const int pathLen = static_cast<int>(strlen(image));
        this->image = new char[pathLen + 1];
        strcpy_s(this->image, pathLen + 1, image);
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
        if (!CanCollideWith(other))
        {
            return false;
        }
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

    bool Actor::CanCollideWith(const Actor* other) const
    {
        if (!other)
        {
            return false;
        }

        if (collisionLayer == Layer_None || other->collisionLayer == Layer_None)
        {
            return false;
        }

        if ((collisionMask & other->collisionLayer) == 0)
        {
            return false;
        }

        if ((other->collisionMask & collisionLayer) == 0)
        {
            return false;
        }

        return true;
    }


    void Actor::ChangeImage(const char* newImage)
    {
        // 기존 메모리 해제.
        SafeDeleteArray(image);

        if (!GetAsciiSizeFromFile(newImage, width, height))
        {
            width = 0;
            height = 0;
            image = nullptr;
            return;
        }

        // 파일 경로 문자열 복사 (이미지 폭이 아니라 경로 길이 기준)
        const int pathLen = static_cast<int>(strlen(newImage));
        image = new char[pathLen + 1];
        strcpy_s(image, pathLen + 1, newImage);
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

