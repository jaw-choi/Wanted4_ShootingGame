#include "Player.h"
#include "Actor/PlayerBullet.h"
#include "Core/Input.h"
#include "Engine/Engine.h"
#include "Level/Level.h"
#include "Render/Renderer.h"

Player::Player()
    : super("<=A=>", Vector2::Zero, Color::Green),
    fireMode(FireMode::OneShot)
{
    // 생성 위치 설정.
    int xPosition = (Engine::Get().GetWidth() / 2) - (width / 2);
    int yPosition = Engine::Get().GetHeight() - 2;
    SetPosition(Vector2(xPosition, yPosition));

    // 타이머 목표 시간 설정.
    timer.SetTargetTime(fireInterval);
    xPostimer.SetTargetTime(moveXInterval);
    yPostimer.SetTargetTime(moveYInterval);
}

Player::~Player()
{
}

void Player::Tick(float deltaTime)
{
    super::Tick(deltaTime);

    //char buffer[256] = {};
    //sprintf_s(
    //    buffer,
    //    256,
    //    "player: (%d, %d)",
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
    // 좌우 방향키 입력처리.
    if (Input::Get().GetKey(VK_LEFT))
    {
        MoveLeftInterval();
    }
    if (Input::Get().GetKey(VK_RIGHT))
    {
        MoveRightInterval();
    }
    if (Input::Get().GetKey(VK_DOWN))
    {
        MoveDownInterval();
    }
    if (Input::Get().GetKey(VK_UP))
    {
        MoveUpInterval();
    }

    // 스페이스 키를 활용해 탄약 발사.
    if (fireMode == FireMode::OneShot)
    {
        if (Input::Get().GetKeyDown(VK_SPACE))
        {
            Fire();
        }
    }
    else if (fireMode == FireMode::Repeat)
    {
        if (Input::Get().GetKey(VK_SPACE))
        {
            FireInterval();
        }
    }

    // 발사 모드 전환.
    if (Input::Get().GetKeyDown('R'))
    {
        int mode = static_cast<int>(fireMode);
        mode = 1 - mode;
        fireMode = static_cast<FireMode>(mode);
    }
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

void Player::Fire()
{
    // 경과 시간 초기화.
    //elapsedTime = 0.0f;
    timer.Reset();

    // 위치 설정.
    Vector2 bulletPosition(
        position.x + (width / 2),
        position.y
    );

    // 액터 생성.
    GetOwner()->AddNewActor(new PlayerBullet(bulletPosition));
}

void Player::FireInterval()
{
    // 발사 가능 여부 확인.
    if (!CanShoot())
    {
        return;
    }

    // 발사.
    Fire();
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

bool Player::CanMoveY() const
{
    // 경과 시간 확인.
    // 발사 간격보다 더 많이 흘렀는지.
    return yPostimer.IsTimeOut();
}
