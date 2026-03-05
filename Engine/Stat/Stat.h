#pragma once
#include "Common/Common.h"

namespace Wanted
{
    // Stat 데이터를 담은 구조체
    struct __declspec(dllexport) Stat
    {
        int hp;
        int maxHp;
        float moveSpeed;
        int power;

        Stat(int hp, int maxHp, float moveSpeed, int power)
            : hp(hp), maxHp(maxHp), moveSpeed(moveSpeed), power(power) {
        }
        // 기본 스탯
        Stat() : hp(100), maxHp(100), moveSpeed(50.0f), power(1) {}
    };

}