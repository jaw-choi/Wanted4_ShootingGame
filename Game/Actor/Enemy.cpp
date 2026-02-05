#include "Enemy.h"
#include "Util/Util.h"
#include "Engine/Engine.h"
#include "Level/Level.h"
#include "Actor/EnemyBullet.h"
#include "Actor/EnemyDestroyEffect.h"


Enemy::Enemy(const char* image)
    : super(image)
{
    int random = Util::Random(0, 10);
    std::vector<Vector2> positions = { {10,10},
	{Engine::Get().GetWidth() - width - 1,0},
	{0,Engine::Get().GetHeight() - height - 1},
	{Engine::Get().GetWidth() - width - 1,Engine::Get().GetHeight() - height - 1} };

    // 이동 방향에 따른 적 위치 설정.
    xPos = (float)positions[random % 4].x;
    yPos = (float)positions[random % 4].y;

    // 발사 타이머 목표 시간 설정.
    timer.SetTargetTime(Util::RandomRange(1.0f, 3.0f));
}

Enemy::~Enemy()
{
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);


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

void Enemy::MoveTo(const Actor& target,float deltaTime)
{
    Vector2 destination = target.GetPosition();

    Vector2f dir = Vector2f(destination - GetPosition()).Normalized();

    xPos += dir.x * moveSpeed * deltaTime;
    yPos += dir.y * moveSpeed * deltaTime;

    // 실제 이동.
    SetPosition(Vector2(
	static_cast<int>(xPos),
	static_cast<int>(yPos)
    ));

}

