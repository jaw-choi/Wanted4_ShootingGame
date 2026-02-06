#pragma once
#include "Common/Common.h"

namespace Wanted
{
    // Stat 데이터를 가지고 이를 다룰 수 있는 객체들을 위한 인터페이스
    class __declspec(dllexport) IStatHolder
    {
    public:
        virtual ~IStatHolder() = default;

        // HP 관련.
        virtual int GetCurrentHP() const = 0;
        virtual int GetMaxHP() const = 0;
        virtual void TakeDamage(int amount) = 0;

        virtual bool IsAlive() const = 0;

        // 이동 속도.
        virtual float GetMoveSpeed() const = 0;

        // 공격력.
        virtual int GetAttackPower() const = 0;
    };

}