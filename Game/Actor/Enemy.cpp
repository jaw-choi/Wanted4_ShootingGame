#include "Enemy.h"
#include "Util/Util.h"
#include "Engine/Engine.h"
#include "Level/Level.h"
#include "Actor/EnemyBullet.h"
#include "Actor/EnemyDestroyEffect.h"

#include "Level/GameLevel.h"
#include "Math/QuadTree.h"

Enemy::Enemy(const char* image)
    : super(image), enemyStats(10, 10, 6.f, 1)
{
    int random = Util::Random(0, 10);
    std::vector<Vector2> positions = { {0,0},
        {Engine::Get().GetWidth() - width - 1,0},
        {0,Engine::Get().GetHeight() - height - 4},
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

    // 1. 다음 위치 예측
    Vector2f desiredPosF = currPos + dir * moveSpeed * deltaTime;
    Vector2 desiredPos = Vector2(static_cast<int>(desiredPosF.x), static_cast<int>(desiredPosF.y));

    // 2. 쿼드트리를 이용한 충돌 검사
    bool collisionDetected = false; // 다른 Enemy와 충돌 감지 여부
    if (GameLevel* gameLevel = dynamic_cast<GameLevel*>(GetOwner()))
    {
        if (Wanted::QuadTree* quadtree = gameLevel->GetQuadtree())
        {
            std::vector<Actor*> potentialColliders;
            // 'this'(현재 Enemy) 주변의 충돌 후보들을 쿼드트리에서 가져옴
            quadtree->Query(potentialColliders, this);
            
            for (Actor* otherActor : potentialColliders)
            {
                // 1. 자기 자신과의 충돌은 검사하지 않습니다.
                    // 2. 비활성화된 액터는 충돌 검사에서 제외합니다.
                    // 3. RTTI_CAST를 사용하여 다른 액터가 'Enemy' 타입인지 확인합니다.
                if (otherActor != this && otherActor->IsActive() && otherActor->IsTypeOf<Enemy>())
                {
                    // 예측된 현재 Enemy의 Bounding Box
                    Rect selfPredictedBounds(desiredPos.x, desiredPos.y, width, height);
                    // 다른 Enemy의 현재 Bounding Box
                    Rect otherEnemyBounds(otherActor->GetPosition().x, otherActor->GetPosition().y,
                        otherActor->GetWidth(), otherActor->GetHeight());

                    // 두 Bounding Box가 겹치는지 확인 (CheckAABBOverlap 함수 대신 Rect::Intersects)
                        if (selfPredictedBounds.Intersects(otherEnemyBounds))
                        {
                            collisionDetected = true;
                            // 충돌이 감지되면 이 Enemy는 이동하지 않을 것이므로 더 이상 검사할 필요 없음
                            break;
                        }
                }
                // Player와의 충돌은 무시하므로 여기에 'else if (RTTI_CAST(Player, otherActor))' 블록
                //합니다.
            }
        }
    }

    // 3. 충돌 여부에 따른 이동 처리
    if (!collisionDetected) // 다른 Enemy와 충돌하지 않았다면
    {
        // 예측된 위치로 실제로 이동합니다.
        currPos = desiredPosF;
    }
    // else: 충돌이 감지되었다면 currPos는 이전 값 그대로 유지되어 이동하지 않습니다.

        // 최종 위치를 Actor의 position에 적용합니다.
    SetPosition(Vector2(
        static_cast<int>(currPos.x),
        static_cast<int>(currPos.y)
    ));



    //// Enemy 이동 //일반 AABB계산
    //currPos = currPos + dir * moveSpeed * deltaTime;
    //SetPosition(Vector2(
    //    static_cast<int>(currPos.x),
    //    static_cast<int>(currPos.y)
    //));

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

