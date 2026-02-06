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

        Stat(int _hp, int _maxHp, float _moveSpeed, int _power)
            : hp(_hp), maxHp(_maxHp), moveSpeed(_moveSpeed), power(_power) {
        }
        // 기본 스탯
        Stat() : hp(100), maxHp(100), moveSpeed(5.0f), power(1) {}
    };

}