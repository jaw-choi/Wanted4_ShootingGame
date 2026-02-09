#include "Player.h"
#include "Actor/PlayerBullet.h"
#include "Core/Input.h"
#include "Engine/Engine.h"
#include "Level/Level.h"
#include "Render/Renderer.h"
#include "Actor/LevelUpOverlay.h"
#include "Level/GameLevel.h"

#include <algorithm>
#include <iostream>

Player::Player()
    : super("<+A+>", Vector2::Zero, Color::Green), playerStats(10, 10, 0.2f, 1), exp(0), currFullExp(10), level(1)
{
    // 생성 위치 설정.
    int xPosition = (Engine::Get().GetWidth() / 2) - (width / 2);
    int yPosition = Engine::Get().GetHeight() / 2 - 2;
    SetPosition(Vector2(xPosition, yPosition));

    // 타이머 목표 시간 설정.
    timer.SetTargetTime(fireInterval);
    xPostimer.SetTargetTime(moveXInterval);
    yPostimer.SetTargetTime(moveYInterval);
    hpTimer.SetTargetTime(invincibilityTime);
    sortingOrder = 5;
}

Player::~Player()
{
}


void Player::Tick(float deltaTime)
{
    if (deltaTime <= 0.0f)
    {
        return;
    }
    super::Tick(deltaTime);

    printHp();
    printExp();
    DrawExpStars(exp,currFullExp);
    DrawHp(GetCurrentHP(), GetMaxHP());
     //player 실시간 position 확인 - 디버그 모드 시
    //char buffer[256] = {};
    //sprintf_s(
    //    buffer,
    //    256,
    //    GetPosition().ToString(),
    //    position.x,
    //    position.y
    //);
    //ChangeImage(buffer);

    // 종료 처리.
    if (Input::Get().GetKeyDown(VK_ESCAPE))
    {
        // 게임 종료.
        QuitGame();
    }

    // 경과 시간 업데이트.
    //elapsedTime += deltaTime;
    timer.Tick(deltaTime);
    xPostimer.Tick(deltaTime);
    yPostimer.Tick(deltaTime);
    hpTimer.Tick(deltaTime);
    // 좌우 방향키 입력처리.
    if (Input::Get().GetKey('A') || Input::Get().GetKey(VK_LEFT))
    {
        MoveLeftInterval();
    }
    if (Input::Get().GetKey('D') || Input::Get().GetKey(VK_RIGHT))
    {
        MoveRightInterval();
    }
    if (Input::Get().GetKey('S') || Input::Get().GetKey(VK_DOWN))
    {
        MoveDownInterval();
    }
    if (Input::Get().GetKey('W') || Input::Get().GetKey(VK_UP))
    {
        MoveUpInterval();
    }

    //FireInterval();
}

void Player::MoveRight()
{
    xPostimer.Reset();
    // 오른쪽 이동 처리.
    position.x += 1;

    // 좌표 검사.
    // "<-=A=->"
    if (position.x + width > Engine::Get().GetWidth())
    {
        position.x -= 1;
    }
}

void Player::MoveLeft()
{
    xPostimer.Reset();
    // 왼쪽 이동 처리.
    position.x -= 1;

    // 좌표 검사.
    if (position.x < 0)
    {
        position.x = 0;
    }
}

void Player::MoveDown()
{
    yPostimer.Reset();
    // 아래쪽 이동 처리.
    position.y += 1;

    // 좌표 검사.
    // "<-=A=->"
    if (position.y + height > Engine::Get().GetHeight())
    {
        position.y -= 1;
    }
}

void Player::MoveUp()
{
    yPostimer.Reset();
    // 위쪽 이동 처리.
    position.y -= 1;

    // 좌표 검사.
    if (position.y < 0)
    {
        position.y = 0;
    }
}

void Player::Fire(Vector2f dir)
{
    // 경과 시간 초기화.
    timer.Reset();

    // 생성 위치 설정.
    Vector2f bulletPosition(
        (float)(position.x + (width / 2)),
        (float)position.y
    );

    // 액터 생성.
    GetOwner()->AddNewActor(new PlayerBullet(bulletPosition,dir));
}


void Player::MoveRightInterval()
{
    // 이동 가능 여부 확인.
    if (!CanMoveX())
    {
        return;
    }

    // 이동 -Y or +Y.
    MoveRight();
}

void Player::MoveLeftInterval()
{
    // 이동 가능 여부 확인.
    if (!CanMoveX())
    {
        return;
    }

    // 이동 -Y or +Y.
    MoveLeft();
}

void Player::MoveUpInterval()
{
    // 이동 가능 여부 확인.
    if (!CanMoveY())
    {
        return;
    }

    // 이동 -Y or +Y.
    MoveUp();
}

void Player::MoveDownInterval()
{
    // 이동 가능 여부 확인.
    if (!CanMoveY())
    {
        return;
    }

    // 이동 -Y or +Y.
    MoveDown();
}



bool Player::CanShoot() const
{
    // 경과 시간 확인.
    // 발사 간격보다 더 많이 흘렀는지.
    return timer.IsTimeOut();
}

bool Player::CanMoveX() const
{
    // 경과 시간 확인.
    // 발사 간격보다 더 많이 흘렀는지.
    return xPostimer.IsTimeOut();
}

void Player::printHp()
{
    sprintf_s(hpString, 128, "Hp: %d", playerStats.hp);
    Renderer::Get().Submit(
        hpString,
        Vector2(0, Engine::Get().GetHeight() - 2)
    );
}

void Player::printExp()
{
    sprintf_s(expString, 128, "Level: %d",level);
    Renderer::Get().Submit(
        expString,
        Vector2(0, Engine::Get().GetHeight() - 4)
    );
}


bool Player::CanMoveY() const
{
    
    // 경과 시간 확인.
    // 발사 간격보다 더 많이 흘렀는지.
    return yPostimer.IsTimeOut();
}

void Player::TakeDamage(int amount)
{
    if (!hpTimer.IsTimeOut())
    {
        return;
    }
    hpTimer.Reset();
    playerStats.hp -= amount;
    //TODO: hit effect -> white>red>white
}

void Player::AddExperience(long long expAmount)
{
    exp += expAmount;
    if (IsExpFull())
    {
        int remain = 0;
        remain = exp % currFullExp;
        LevelUp(remain);
        Level* currLevel = GetOwner();
        GameLevel* gameLevel = dynamic_cast<GameLevel*>(GetOwner());
        gameLevel->ShowLevelUpUI();
        dynamic_cast<GameLevel*>(GetOwner())->ShowLevelUpUI();
        dynamic_cast<GameLevel*>(GetOwner())->AddNewActor(new LevelUpOverlay(this));
    }
}

bool Player::IsDead() const
{
    return playerStats.hp <= 0;
}

bool Player::IsExpFull() const
{
    return exp >= currFullExp;
}

void Player::LevelUp(int remain)
{
    // exp 0으로 만들기
    exp = remain;
    // currFullExp 다음 단계로 올리기
    currFullExp = (long long)((float)currFullExp * 1.2f);
    ++level;
    if (onLevelUp)
        onLevelUp();
}

// Level에서 매 tick 불림
void Player::AutoFireAt(const Actor& target)
{
    if (!CanShoot())
    {
        return;
    }

    //자동으로 enemy 방향으로 공격
    Vector2f dir = Vector2f((Vector2f)target.GetPosition() - (Vector2f)GetPosition()).Normalized();
    Fire(dir);
}

void Player::AutoFireAtMouse()
{
    if (!CanShoot())
    {
        return;
    }

    //자동으로 mouse 방향으로 공격
    Vector2f dir = Vector2f((Vector2f)Input::Get().MousePosition() - (Vector2f)GetPosition()).Normalized();

    Fire(dir);
}

static double Clamp01(double v)
{
    if (v < 0.0) return 0.0;
    if (v > 1.0) return 1.0;
    return v;
}


void Player::DrawExpStars(int exp, int currFullExp)
{
    const int screenW = Engine::Get().GetWidth();
    if (screenW <= 0) return;

    // 버퍼 오버런 방지: 화면폭을 버퍼 크기-1로 제한
    const int bufW = min(screenW, (int)sizeof(expBarString) - 1);

    double ratio = 0.0;
    if (currFullExp > 0)
        ratio = Clamp01((double)exp / (double)currFullExp);

    int filled = (int)std::floor(ratio * bufW);
    if (filled < 0) filled = 0;
    if (filled > bufW) filled = bufW;

    // 1) 먼저 공백으로 줄 전체 채움
    memset(expBarString, ' ', bufW);

    // 2) 채워진 만큼만 별로 덮어쓰기
    memset(expBarString, '*', filled);

    // 3) 널 종료
    expBarString[bufW] = '\0';

    // 4) 화면 상단 출력
    Renderer::Get().Submit(expBarString, Vector2(0, 0),Color::Blue,2);
}


void Player::DrawHp(int hp, int fullHp)
{
    const int screenW = Engine::Get().GetWidth();
    if (screenW <= 0) return;

    // 버퍼 오버런 방지: 화면폭을 버퍼 크기-1로 제한
    const int bufW = min(screenW, (int)sizeof(hpBarString) - 1);

    double ratio = 0.0;
    if (fullHp > 0)
        ratio = Clamp01((double)hp / (double)fullHp);

    int filled = (int)std::floor(ratio * bufW);
    if (filled < 0) filled = 0;
    if (filled > bufW) filled = bufW;

    // 1) 먼저 공백으로 줄 전체 채움
    memset(hpBarString, ' ', bufW);

    // 2) 채워진 만큼만 별로 덮어쓰기
    memset(hpBarString, '*', filled);

    // 3) 널 종료
    hpBarString[bufW] = '\0';

    // 4) 화면 상단 출력
    Renderer::Get().Submit(hpBarString, Vector2(0, 1), Color::Red,2);
}