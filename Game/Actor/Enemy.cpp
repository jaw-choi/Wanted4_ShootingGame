#include "Enemy.h"
#include "Util/Util.h"
#include "Engine/Engine.h"
#include "Level/Level.h"
#include "Actor/EnemyBullet.h"
#include "Actor/EnemyDestroyEffect.h"
#include "Actor/Player.h"
#include "Actor/ExpGem.h"

#include "Level/GameLevel.h"
#include "Math/QuadTree.h"

#include <crtdbg.h>
#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif

std::vector<Enemy*> Enemy::pool;



Enemy::Enemy(const char* image)
    : super(image), enemyStats(10, 10, 50.f, 1)
{
    int random = Util::Random(0, 15);
    
    int screenW = Engine::Get().GetWidth();
    int screenH = Engine::Get().GetHeight();

    int maxX = screenW - width - 1;
    int maxY = screenH - height - 1;

    // UI 때문에 아래쪽/위쪽을 비워야 하면 여기에서 조정하세요.
    // 예: 하단 UI 4줄을 비우는 경우
    int uiBottomMargin = 4;
    maxY = screenH - height - uiBottomMargin;

    int midX = maxX / 2;
    int midY = maxY / 2;

    std::vector<Vector2> positions;
    positions.reserve(16);

    const int n = 5;          // 5x5 둘레 = 16
    const int steps = n - 1;  // 4

    auto LerpInt = [](int a, int b, int i, int steps)
	{
	    return a + (b - a) * i / steps;
	};

    // Top edge (y=0): 5
    for (int i = 0; i < n; ++i)
    {
	int x = LerpInt(0, maxX, i, steps);
	positions.push_back({ x, 0 });
    }

    // Right edge (x=maxX): 3 (코너 중복 제거)
    for (int i = 1; i < n - 1; ++i)
    {
	int y = LerpInt(0, maxY, i, steps);
	positions.push_back({ maxX, y });
    }

    // Bottom edge (y=maxY): 5
    for (int i = 0; i < n; ++i)
    {
	int x = LerpInt(0, maxX, i, steps);
	positions.push_back({ x, maxY });
    }

    // Left edge (x=0): 3 (코너 중복 제거)
    for (int i = 1; i < n - 1; ++i)
    {
	int y = LerpInt(0, maxY, i, steps);
	positions.push_back({ 0, y });
    }

    // positions.size() == 16


    // 이동 방향에 따른 적 위치 설정.
    currPos.x = (float)positions[random % positions.size()].x;
    currPos.y = (float)positions[random % positions.size()].y;
    dir = Vector2f::Zero;
    // 발사 타이머 목표 시간 설정.
    //shotTimer.SetTargetTime(Util::RandomRange(1.0f, 3.0f));
}

Enemy::~Enemy()
{
}

Enemy* Enemy::Acquire(Level* owner, const char* image, const Vector2& spawnPosition)
{
	Enemy* enemy = nullptr;
	if (!pool.empty())
	{
		enemy = pool.back();
		pool.pop_back();
		enemy->inPool = false;
		enemy->Initialize(image, spawnPosition);
	}
	else
	{
		enemy = new Enemy(image);
		enemy->SetWorldPosition(spawnPosition);
	}

	if (owner && enemy->GetOwner() == nullptr)
	{
		owner->AddNewActor(enemy);
	}

	return enemy;
}

void Enemy::Prewarm(Level* owner, int count)
{
	if (count <= 0)
	{
		return;
	}

	for (int i = 0; i < count; ++i)
	{
		Enemy* enemy = new Enemy("(oOo)");
		enemy->inPool = true;
		enemy->isActive = false;
		enemy->destroyRequested = false;
		pool.emplace_back(enemy);

		if (owner)
		{
			owner->AddNewActor(enemy);
		}
	}
}

void Enemy::ReleaseToPool()
{
	if (inPool)
	{
		return;
	}

	inPool = true;
	isActive = false;
	destroyRequested = false;

	pool.emplace_back(this);
}

void Enemy::Initialize(const char* image, const Vector2& spawnPosition)
{
	isActive = true;
	destroyRequested = false;

	ChangeImage(image);

	moveSpeed = enemyStats.moveSpeed;
	direction = MoveDirection::None;
	dir = Vector2f::Zero;

	currPos = Vector2f(spawnPosition);
	SetPosition(spawnPosition);

	shotTimer.SetTargetTime(Util::RandomRange(1.0f, 3.0f));
	shotTimer.Reset();
}

void Enemy::Tick(float deltaTime)
{
    super::Tick(deltaTime);
    if (deltaTime <= 0.0f)
    {
        return;
    }
    // 1. 다음 위치 예측
    float yScale = 0.6f; // 세로는 60% 속도

    Vector2f delta = dir * (moveSpeed * deltaTime);
    delta.y *= yScale;

    currPos = currPos + delta;
    //Vector2 desiredPos = Vector2(static_cast<int>(desiredPosF.x), static_cast<int>(desiredPosF.y));

    //// 2. 쿼드트리를 이용한 충돌 검사
    //bool collisionDetected = false; // 다른 Enemy와 충돌 감지 여부
    //Actor* collidingEnemy = nullptr;
    //if (GameLevel* gameLevel = dynamic_cast<GameLevel*>(GetOwner()))
    //{
    //    QuadTree* quadtree = gameLevel->GetQuadtree();
    //    if (quadtree && this)
    //    {
    //        std::vector<Actor*> potentialColliders;
    //        // 'this'(현재 Enemy) 주변의 충돌 후보들을 쿼드트리에서 가져옴
    //        quadtree->Query(potentialColliders, this);

    //        for (Actor* otherActor : potentialColliders)
    //        {
    //            // 1. 자기 자신과의 충돌은 검사하지 않음.
    //                // 2. 비활성화된 액터는 충돌 검사에서 제외.
    //                // 3. RTTI_CAST를 사용하여 다른 액터가 'Enemy' 타입인지 확인.
    //            if (otherActor != this && otherActor->IsActive() && otherActor->IsTypeOf<Enemy>())
    //            {
    //                // 예측된 현재 Enemy의 Bounding Box
    //                Rect selfPredictedBounds(desiredPos.x, desiredPos.y, width, height);
    //                // 다른 Enemy의 현재 Bounding Box
    //                Rect otherEnemyBounds(otherActor->GetPosition().x, otherActor->GetPosition().y,
    //                    otherActor->GetWidth(), otherActor->GetHeight());

    //                // 두 Bounding Box가 겹치는지 확인 (CheckAABBOverlap 함수 대신 Rect::Intersects)
    //                if (selfPredictedBounds.Intersects(otherEnemyBounds))
    //                {
    //                    collisionDetected = true;
    //                    collidingEnemy = otherActor;
    //                    // 충돌이 감지되면 이 Enemy는 더 이상 검사할 필요 없음
    //                    break;
    //                }
    //            }

    //        }
    //    }
    //}

    //// 3. 충돌 여부에 따른 이동 처리
    //if (!collisionDetected) // 다른 Enemy와 충돌하지 않았다면
    //{
    //    // 예측된 위치로 실제로 이동합니다.
    //    currPos = desiredPosF;
    //}
    //else // 충돌이 감지되었다면, 멈추는 대신 비켜가려는 시도
    //{
    //    if (collidingEnemy) // 충돌한 적이 있다면
    //    {
    //        // 충돌한 적의 위치를 기준으로 반대 방향으로 분리 벡터를 계산합니다.
    //        Vector2f separationVector = (currPos - (Vector2f)collidingEnemy->GetPosition()).Normalized();
    //        // 기존 진행 방향(dir)에 분리 벡터를 살짝 더하여 새로운 방향을 시도합니다.
    //            // 0.2f는 분리 힘의 강도입니다. 이 값을 조절하여 적절한 움직임을 찾을 수 있습니다.
    //        dir = (dir + separationVector * 0.2f).Normalized();
    //        // 변경된 방향으로 이동을 시도합니다.
    //        currPos = currPos + dir * moveSpeed * deltaTime;
    //    }
    //    // else: collidingEnemy가 없는 경우 (예외 상황)에는 이전 값 유지 (멈춤)
    //}

        // 최종 위치를 Actor의 position에 적용합니다.
    SetPosition(Vector2(
        static_cast<int>(currPos.x),
        static_cast<int>(currPos.y)
    ));

}

void Enemy::Draw()
{
    Renderer::Get().SubmitFromFile(image, position, color, sortingOrder);
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
    if (!IsActive())
    {
        return;
    }

    // 액터 제거.
    ReleaseToPool();

    // 이펙트 생성 (재생을 위해).
    GetOwner()->AddNewActor(new EnemyDestroyEffect(position));

    //Exp 생성
    //ExpGem::Acquire(GetOwner(), position);
}

void Enemy::MoveTo(const Actor& target)
{
    MoveToPosition(target.GetPosition());
}

void Enemy::MoveToPosition(const Vector2& targetPosition)
{
    Vector2f destination = (Vector2f)targetPosition;
    dir = Vector2f(destination - currPos).Normalized();
}

void Enemy::SetWorldPosition(const Vector2& newPosition)
{
    currPos = Vector2f(newPosition);
    SetPosition(newPosition);
}



