#include "Enemy.h"
#include "Util/Util.h"
#include "Engine/Engine.h"
#include "Level/Level.h"
#include "Actor/EnemyBullet.h"
#include "Actor/EnemyDestroyEffect.h"


Enemy::Enemy(const char* image)
    : super(image), enemyStats(10, 10, 6.f, 1)
{
    int random = Util::Random(0, 10);
    std::vector<Vector2> positions = { {0,0},
	{Engine::Get().GetWidth() - width - 1,0},
	{0,Engine::Get().GetHeight() - height - 1},
	{Engine::Get().GetWidth() - width - 1,Engine::Get().GetHeight() - height - 1} };

    // 이동 방향에 따른 적 위치 설정.
    currPos.x = (float)positions[random % 4].x;
    currPos.y = (float)positions[random % 4].y;
    dir = Vector2f::Zero;
    // 발사 타이머 목표 시간 설정.
    timer.SetTargetTime(Util::RandomRange(1.0f, 3.0f));
}

Enemy::~Enemy()
{
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// Enemy 이동
	currPos = currPos + dir * moveSpeed * deltaTime;

	SetPosition(Vector2(
	    static_cast<int>(currPos.x),
	    static_cast<int>(currPos.y)
	));

	// Todo: 보스 enemy에 적용예정
	//// 발사 타이머 업데이트.
	//timer.Tick(deltaTime);
	//if (!timer.IsTimeOut())
	//{
	//	return;
	//}

	//// 타이머 리셋.
	//timer.Reset();

	//// 탄약 발사.
	//GetOwner()->AddNewActor(new EnemyBullet(
	//	Vector2(position.x + width / 2, position.y + height / 2),
	//	Util::RandomRange(10.0f, 20.0f)
	//));
}

void Enemy::TakeDamage(int amount)
{
    enemyStats.hp -= amount;
}

bool Enemy::IsDead() const
{
    return enemyStats.hp <= 0;
}

void Enemy::OnDamaged()
{
	// 액터 제거.
	Destroy();

	// 이펙트 생성 (재생을 위해).
	GetOwner()->AddNewActor(new EnemyDestroyEffect(position));
}

void Enemy::MoveTo(const Actor& target)
{
    Vector2f destination = (Vector2f)target.GetPosition();

    dir = Vector2f(destination - currPos).Normalized();

    
}

