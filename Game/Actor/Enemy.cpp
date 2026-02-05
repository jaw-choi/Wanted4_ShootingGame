#include "Enemy.h"
#include "Util/Util.h"
#include "Engine/Engine.h"
#include "Level/Level.h"
#include "Actor/EnemyBullet.h"
#include "Actor/EnemyDestroyEffect.h"

Enemy::Enemy(const char* image)
	: super(image)
{
	// 랜덤 (오른쪽 또는 왼쪽으로 이동할지 결정).
	int random = Util::Random(1, 10);

	if (random % 4 == 0)
	{
		// 화면 오른쪽에서 생성. "(oOo)"
		direction = MoveDirection::Left;
		xPos = static_cast<float>(
			Engine::Get().GetWidth() - width - 1
			);
	}
	else if (random % 4 == 1)
	{
		// 화면 왼쪽에서 생성.
		direction = MoveDirection::Right;
		xPos = 0.0f;
	}
	else if (random % 4 == 2)
	{
	    // 화면 아래쪽에서 생성.
	    direction = MoveDirection::Up;
	    yPos = static_cast<float>(
		Engine::Get().GetHeight() - height - 1
		);;
	}
	else
	{
	    // 화면 위쪽에서 생성.
	    direction = MoveDirection::Down;
	    yPos = 0.0f;
	}

	// 이동 방향에 따른 적 위치 설정.
	SetPosition(
		Vector2(static_cast<int>(xPos), static_cast<int>(yPos))
	);

	// 발사 타이머 목표 시간 설정.
	timer.SetTargetTime(Util::RandomRange(1.0f, 3.0f));
}

Enemy::~Enemy()
{
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 이동 처리.
	float xDir = 0.f, yDir = 0.f;
	if (direction == MoveDirection::Left)
	    xDir = -1.f;
	else if (direction == MoveDirection::Right)
	    xDir = 1.f;
	else if (direction == MoveDirection::Up)
	    yDir = -1.f;
	else
	    yDir = 1.f;
	//xDir = direction == MoveDirection::Left ? -1.0f : 0.0f;
	//xDir = direction == MoveDirection::Right ? 1.0f : 0.0f;
	xPos = xPos + moveSpeed * xDir * deltaTime;

	//yDir = direction == MoveDirection::Up ? -1.0f : 0.0f;
	//yDir = direction == MoveDirection::Down ? 1.0f : 0.0f;
	yPos = yPos + moveSpeed * yDir * deltaTime;

	// 좌표 검사.
	// 화면 왼쪽을 완전히 벗어났으면.
	if (xPos + width < 0)
	{
		Destroy();
		return;
	}

	// 화면 오른쪽을 완전히 벗어났으면.
	if (xPos > Engine::Get().GetWidth() - 1)
	{
		Destroy();
		return;
	}

	// 화면 위쪽을 완전히 벗어났으면.
	if (yPos + height < 0)
	{
	    Destroy();
	    return;
	}

	// 화면 아래쪽을 완전히 벗어났으면.
	if (yPos > Engine::Get().GetHeight() - 1)
	{
	    Destroy();
	    return;
	}


	// 위치 설정.
	SetPosition(Vector2(
	    static_cast<int>(xPos),
	    static_cast<int>(yPos)
	));

	// 발사 타이머 업데이트.
	timer.Tick(deltaTime);
	if (!timer.IsTimeOut())
	{
		return;
	}

	// 타이머 리셋.
	timer.Reset();

	// 탄약 발사.
	GetOwner()->AddNewActor(new EnemyBullet(
		Vector2(position.x + width / 2, position.y + height / 2),
		Util::RandomRange(10.0f, 20.0f)
	));
}

void Enemy::OnDamaged()
{
	// 액터 제거.
	Destroy();

	// 이펙트 생성 (재생을 위해).
	GetOwner()->AddNewActor(new EnemyDestroyEffect(position));
}
