#include "GameLevel.h"
#include "Actor/Player.h"
#include "Actor/Enemy.h"
#include "Actor/PlayerBullet.h"
#include "Actor/EnemyBullet.h"
#include "Actor/EnemySpawner.h"
#include "Actor/MouseTester.h"
#include "Actor/ExpGem.h"
#include "Actor/LevelUpOverlay.h"
#include "Render/Renderer.h"
#include "Engine/Engine.h"

#include <set>

GameLevel::GameLevel()
{
    // Player 액터 추가.
    AddNewActor(new Player());

    // 적 생성기 추가.
    AddNewActor(new EnemySpawner());

    // Test: 마우스 테스터 추가.
    AddNewActor(new MouseTester());
    quadtree = new QuadTree(Rect(0, 0, Engine::Get().GetWidth(), Engine::Get().GetHeight()), 4, 0, 5);

}

GameLevel::~GameLevel()
{
}

void GameLevel::PrintFPS(float deltaTime)
{
    static double accumTime = 0.0;
    static int frameCount = 0;
    static double cachedFps = 0.0;

    accumTime += deltaTime;
    frameCount++;

    if (accumTime >= 0.01)
    {
        cachedFps = (double)frameCount / accumTime;
        accumTime = 0.0;
        frameCount = 0;
    }

    static char fpsString[128]; // ★ 핵심: static
    sprintf_s(fpsString, 128, "FPS: %.1f  dt: %.4f", cachedFps, deltaTime);

    Renderer::Get().Submit(
        fpsString,
        Vector2(0, Engine::Get().GetHeight() - 3)
    );
}

void GameLevel::MakeQuadTree()
{
    //quadtree 형성
    quadtree->Clear();

    // 액터 필터링.
    for (Actor* const actor : actors)
    {
        // 모든 active actor tree에 insert.
        if (actor->IsActive())
            quadtree->Insert(actor);
    }


}
void GameLevel::Tick(float deltaTime)
{
    //super::Tick(deltaTime);

    // QuadTree 생성.
    MakeQuadTree();

    // 충돌 판정 처리.
    ProcessCollisionPlayerBulletAndEnemy();
    //ProcessCollisionPlayerAndEnemyAABB();
    ProcessCollisionPlayerAndEnemyQuadTree();
    ProcessCollisionPlayerAndExpGemQuadTree();
    ProcessCollisionPlayerAndEnemyBullet();
    ProcessAstarAlgorithmPlayerAndEnemy();


    ///
    PrintFPS(deltaTime);


        //Level Up UI
        for (Actor* const actor : actors)
        {
            if (isLevelUpUIVisible)
            {
                if (actor->IsTypeOf<LevelUpOverlay>())
                {
                    actor->Tick(deltaTime);
                }
                else
                    actor->Tick(0.f);
            }
            else
            {
                actor->Tick(deltaTime);
            }
        }
    
}

void GameLevel::Draw()
{
    super::Draw();

    if (isPlayerDead)
    {
        // 플레이어 죽음 메시지 Renderer에 제출.
        Renderer::Get().Submit("!Dead!", playerDeadPosition);

        // 점수 보여주기.
        ShowScore();

        // 화면에 바로 표시.
        Renderer::Get().PresentImmediately();

        // 프로그램 정지.
        Sleep(2000);

        // 게임 종료.
        Engine::Get().QuitEngine();
    }

    // 점수 보여주기.
    ShowScore();
}

void GameLevel::ProcessCollisionPlayerBulletAndEnemy()
{
    // 플레이어 탄약과 적 액터 필터링.
    std::vector<Actor*> bullets;
    std::vector<Enemy*> enemies;

    // 액터 필터링.
    for (Actor* const actor : actors)
    {
        if (actor->IsTypeOf<PlayerBullet>())
        {
            bullets.emplace_back(actor);
            continue;
        }

        if (actor->IsTypeOf<Enemy>())
        {
            enemies.emplace_back(actor->As<Enemy>());
        }
    }

    // 판정 안해도 되는지 확인.
    if (bullets.size() == 0 || enemies.size() == 0)
    {
        return;
    }

    // 충돌 판정.
    for (Actor* const bullet : bullets)
    {
        for (Enemy* const enemy : enemies)
        {
            // AABB 겹침 판정.
            if (bullet->TestIntersect(enemy))
            {
                enemy->OnDamaged();
                bullet->Destroy();

                // 점수 추가.
                score += 1;
                continue;
            }
        }
    }
}

void GameLevel::ProcessCollisionPlayerAndEnemyAABB()
{
    // 플레이어와 적 액터 필터링.
    Player* player = nullptr;
    std::vector<Enemy*> enemies;

    // 액터 필터링.
    for (Actor* const actor : actors)
    {
        if (!player && actor->IsTypeOf<Player>())
        {
            player = actor->As<Player>();
            continue;
        }

        if (actor->IsTypeOf<Enemy>())
        {
            enemies.emplace_back(actor->As<Enemy>());
        }
    }

    // 판정 안해도 되는지 확인.
    if (!player || enemies.size() == 0)
    {
        return;
    }

    // 충돌 판정.
    for (Actor* const enemy : enemies)
    {
        if (enemy->TestIntersect(player))
        {
            //player의 Hp 감소
            //충돌 시 TakeDamage
            player->TakeDamage(static_cast<Enemy*>(enemy)->GetAttackPower());

            //// 플레이어 죽음 설정.
            if (player->IsDead()) {
                isPlayerDead = true;
                // 죽은 위치 저장.
                playerDeadPosition = player->GetPosition();

                // 액터 제거 처리.
                player->Destroy();

            }

            //enemy->Destroy();
            //break;
        }
    }
}

void GameLevel::ProcessCollisionPlayerAndEnemyQuadTree()
{

    // 플레이어와 적 액터 필터링.
    Player* player = nullptr;
    std::vector<Actor*> otherActors;

    // 액터 필터링.
    for (Actor* const actor : actors)
    {
        // player actor 찾기
        if (!player && actor->IsTypeOf<Player>())
        {
            player = actor->As<Player>();
            continue;
        }
    }

    if (quadtree && player)
    {
        // Query로 tree에서 Enemy들에 대한 정보를 가져옴
        quadtree->Query(otherActors, player);
        for (Actor* const actor : otherActors)
        {
            if (actor->IsTypeOf<Enemy>())
            {
                if (actor->TestIntersect(player))
                {
                    //player의 Hp 감소
                    //충돌 시 TakeDamage
                    player->TakeDamage(static_cast<Enemy*>(actor)->GetAttackPower());

                    //// 플레이어 죽음 설정.
                    if (player->IsDead()) {
                        isPlayerDead = true;
                        // 죽은 위치 저장.
                        playerDeadPosition = player->GetPosition();

                        // 액터 제거 처리.
                        player->Destroy();
                    }
                }
            }
        }
    }
}

void GameLevel::ProcessCollisionPlayerAndExpGemQuadTree()
{
    // 플레이어와 적 액터 필터링.
    Player* player = nullptr;
    std::vector<Actor*> otherActors;

    // 액터 필터링.
    for (Actor* const actor : actors)
    {
        // player actor 찾기
        if (!player && actor->IsTypeOf<Player>())
        {
            player = actor->As<Player>();
            continue;
        }
    }

    if (quadtree && player)
    {
        // Query로 tree에서 Enemy들에 대한 정보를 가져옴
        quadtree->Query(otherActors, player);
        for (Actor* const actor : otherActors)
        {
            if (actor->IsTypeOf<ExpGem>())
            {
                if (actor->TestIntersect(player))
                {
                    //충돌 시 Get Exp and destroy Exp
                    player->AddExperience(10);
                    actor->Destroy();
                }
            }
        }
    }

}



void GameLevel::ProcessCollisionPlayerAndEnemyBullet()
{
    // 액터 필터링을 위한 변수.
    Player* player = nullptr;
    std::vector<Actor*> bullets;

    // 액터 필터링.
    for (Actor* const actor : actors)
    {
        if (!player && actor->IsTypeOf<Player>())
        {
            player = actor->As<Player>();
            continue;
        }

        if (actor->IsTypeOf<EnemyBullet>())
        {
            bullets.emplace_back(actor);
        }
    }

    // 판정 처리 안해도 되는지 확인.
    if (bullets.size() == 0 || !player)
    {
        return;
    }

    // 충돌 판정.
    for (Actor* const bullet : bullets)
    {
        if (bullet->TestIntersect(player))
        {
            // 플레이어 죽음 설정.
            isPlayerDead = true;

            // 죽은 위치 저장.
            playerDeadPosition = player->GetPosition();

            // 액터 제거 처리.
            player->Destroy();
            bullet->Destroy();
            break;
        }
    }
}

void GameLevel::ShowScore()
{
    sprintf_s(scoreString, 128, "Score: %d", score);
    Renderer::Get().Submit(
        scoreString,
        Vector2(0, Engine::Get().GetHeight() - 1)
    );

}

void GameLevel::ProcessAstarAlgorithmPlayerAndEnemy()
{
    // 플레이어 탄약과 적 액터 필터링.
    Player* player = nullptr;
    std::vector<Enemy*> enemies;

    // 액터 필터링.
    for (Actor* const actor : actors)
    {
        if (!player && actor->IsTypeOf<Player>())
        {
            player = actor->As<Player>();
            continue;
        }

        if (actor->IsTypeOf<Enemy>())
        {
            enemies.emplace_back(actor->As<Enemy>());
        }
    }

    // 판정 안해도 되는지 확인.
    if (!player || enemies.size() == 0)
    {
        return;
    }

    //enemy와 그것의 거리를 pair 로 묶어서 set으로 정리하면 distance 크기 순으로 자동정렬됨
    //distance가 같으면 중복제거 되기 때문에 multiset 쓰겠음
    //pair에서 first를 거리로 넣어서 거리순으로 정렬 되게 함
    //std::multiset<std::pair<float, Actor*>> distances;
    //dir만 전달
    Enemy* closestEnemy = nullptr;
    float minDistSq = FLT_MAX;
    for (Actor* const enemy : enemies)
    {
        // Todo: A* algorithm 계산으로 변경
        // 

        // min dist인 Enemy 구하기
        float distSq = Vector2f(enemy->GetPosition() - player->GetPosition()).LengthSquared();

        if (distSq < minDistSq)
        {
            minDistSq = distSq;
            closestEnemy = static_cast<Enemy*>(enemy);
        }


        //actor -> Enemy 다운캐스팅 체크
        //Enemy* ptrEnemy = dynamic_cast<Enemy*>(enemy);
        //위와 같은 효과
        //if (enemy->IsTypeOf<Enemy>())
        //이미 Enemy이기 때문에
        static_cast<Enemy*>(enemy)->MoveTo(*player);
    }
    if (closestEnemy)
    {
        player->AutoFireAt(*closestEnemy);
    }

}

