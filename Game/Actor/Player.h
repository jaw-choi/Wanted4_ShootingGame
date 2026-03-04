#pragma once

#include "Actor/Actor.h"
#include "Util/Timer.h"
#include "Stat/Stat.h"
#include "Interface/IstatHolder.h"
#include <functional>

using namespace Wanted;

class Player : public Actor, public IStatHolder
{
    // 발사 모드.
    enum class FireMode
    {
        None = -1,
        OneShot,
        Repeat
    };

    RTTI_DECLARATIONS(Player, Actor)

public:
    Player();
    virtual ~Player();
public:

    inline void SetCurrentHP(int hp) { playerStats.hp = hp; }
    inline void SetFullExp(int exp) { currFullExp = exp; }
    // --- IStatHolder 인터페이스 구현 ---
    virtual inline int GetCurrentHP() const override { return playerStats.hp; }
    virtual inline int GetMaxHP() const override { return playerStats.maxHp; }
    virtual void TakeDamage(int amount) override;
    //virtual void Heal(int amount) override;
    virtual inline bool IsAlive() const override { return playerStats.hp > 0; }
    virtual inline float GetMoveSpeed() const override { return playerStats.moveSpeed; }
    virtual inline int GetAttackPower() const override { return playerStats.power; }

    // --- Player 전용 스탯 및 기능 ---
    long long inline GetExperience() const { return exp; }
    void AddExperience(long long expAmount);
    bool IsDead() const;
    bool IsExpFull() const;
    void LevelUp(int remain);

public:
    //가장 가까운 적에게 발사
    void AutoFireAt(const Actor& target);

    //마우스 포인터 쪽으로 발사
    void AutoFireAtMouse();

    //LevelUpOverlay에서 호출하는 함수들
    void SpeedUp();
    void ShotTimeLevelUp();
    void BulletSpeedLevelUp();

    //Exp UI
    void DrawExpStars(long long exp, long long currFullExp);
    void DrawHp(int hp,int fullHp);

    // Level이 등록해주는 콜백
    void SetOnLevelUp(const std::function<void()>& callback) { onLevelUp = callback; }

private:

    virtual void Tick(float deltaTime) override;
    virtual void Draw() override;

    // 오른쪽으로 이동하는 함수.
    void MoveRight();

    // 왼쪽으로 이동하는 함수.
    void MoveLeft();

    // 아래로 이동하는 함수.
    void MoveDown();

    // 위로 이동하는 함수.
    void MoveUp();

    // 탄약 발사 함수.
    void Fire(Vector2f dir);

    // 연속 이동 함수.
    void MoveUpInterval();
    void MoveDownInterval();
    void MoveLeftInterval();
    void MoveRightInterval();

    // 발사 가능여부 확인 함수.
    bool CanShoot() const;

    // Y축 이동 가능여부 확인 함수.
    bool CanMoveY() const;
    bool CanMoveX() const;

    void PrintF(const char* str, float stat, int x, int y);
    void PrintI(const char* str, int stat, int x, int y);
    void PrintHp();
    void PrintSpeed();
    void PrintExp();
    void PrintBulletSpeed();
    void PrintShotSpeed();

    void PlayAnimHit();
private:
    std::function<void()> onLevelUp;

    // 타이머 변수.
    Timer shotTimer;
    Timer yPostimer;
    Timer xPostimer;
    Timer hpTimer;

    bool isTakenDamage = false;

private:
    // 연사 시간 간격.
    float fireInterval = 2.f;

    // 연속 이동 시간 간격. 
    float moveXInterval = 0.05f;
    float moveYInterval = 0.05f;

    // 피격 시 무적 시간.
    float invincibilityTime = 1.f;


    // 플레이어 스탯.
    Stat playerStats;
    long long exp;
    long long currFullExp;
    int bulletNum = 1;
    int level;
    float bulletSpeed = 10.f;


    char buffer[128] = {};
    char hpString[128] = {};
    char speedString[128] = {};
    char bulletNumString[128] = {};
    char bulletShotSpeedString[128] = {};
    char expString[128] = {};
    char expBarString[128] = {};
    char hpBarString[128] = {};

    float hitElapsedTime = 0.f;

};
