#pragma once

#include "Actor/Actor.h"
#include "Util/Timer.h"
#include "Interface/IstatHolder.h"
#include "Stat/Stat.h"
#include <vector>

using namespace Wanted;

class Enemy : public Actor, public IStatHolder
{
    RTTI_DECLARATIONS(Enemy, Actor)

        // 이동 방향 열거형.
        enum class MoveDirection
    {
        None = -1,
        Left,
        Right,
        Up,
        Down
    };

public:
    Enemy(const char* image = "(oOo)");
    virtual ~Enemy();

    // Tick.
    virtual void Tick(float deltaTime) override;
public:
    static Enemy* Acquire(Level* owner, const char* image, const Vector2& spawnPosition);
    static void Prewarm(Level* owner, int count);
    void ReleaseToPool();
    void Initialize(const char* image, const Vector2& spawnPosition);

    // --- IStatHolder 인터페이스 구현 ---
    virtual inline int GetCurrentHP() const override { return enemyStats.hp; }
    virtual inline int GetMaxHP() const override { return enemyStats.maxHp; }
    virtual void TakeDamage(int amount) override;
    //virtual void Heal(int amount) override;
    virtual inline bool IsAlive() const override { return enemyStats.hp > 0; }
    virtual inline float GetMoveSpeed() const override { return enemyStats.moveSpeed; }
    virtual inline int GetAttackPower() const override { return enemyStats.power; }

    bool IsDead() const;


    // 대미지 받았을 때 처리할 함수.
    void OnDamaged();

    void MoveTo(const Actor& target);
    void MoveToPosition(const Vector2& targetPosition);
    void SetWorldPosition(const Vector2& newPosition);
private:
    // 이동 방향 열거형.
    MoveDirection direction = MoveDirection::None;

    // 좌우 이동 처리를 위한 변수.

    float moveSpeed = 6.f;

    Vector2f dir;
    Vector2f currPos;

    bool inPool = false;

    static std::vector<Enemy*> pool;

    // 발사 타이머.
    Timer shotTimer;
private:
    // 스탯 구조체
    Stat enemyStats;

};
