#include "GameLevel.h"
#include "Actor/Player.h"
#include "Actor/Enemy.h"
#include "Actor/PlayerBullet.h"
#include "Actor/EnemyBullet.h"
#include "Actor/EnemySpawner.h"
#include "Actor/MouseTester.h"
#include "Actor/ExpGem.h"
#include "Actor/LevelUpOverlay.h"
#include "Actor/Obstacle.h"
#include "Render/Renderer.h"
#include "Engine/Engine.h"
#include "Core/Input.h"
#include "Game/Game.h"

#include <set>
#include <algorithm>
#include <iostream>

#include <crtdbg.h>
#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif

static void ClearRect(const Vector2& topLeft, int w, int h, Color bg, int z)
{
    if (w <= 0 || h <= 0) return;

    // 최대 폭만큼 공백 스트링을 만들어 두고 잘라 쓰기
    static std::string spaces;
    if ((int)spaces.size() < w)
        spaces.assign(w, ' ');

    const char* line = spaces.c_str(); // 길이 w만큼 공백

    for (int y = 0; y < h; ++y)
    {
        Renderer::Get().Submit(
            line,
            Vector2(topLeft.x, topLeft.y + y),
            bg,
            z
        );
    }
}

static void DrawBox(const Vector2& topLeft, int w, int h, Color color, int z)
{
    if (w < 2 || h < 2) return;

    std::string top(w, '#');
    std::string mid(w, ' ');
    mid.front() = '#';
    mid.back() = '#';

    Renderer::Get().Submit(top.c_str(), Vector2(topLeft.x, topLeft.y), color, z);
    for (int y = 1; y < h - 1; ++y)
        Renderer::Get().Submit(mid.c_str(), Vector2(topLeft.x, topLeft.y + y), color, z);
    Renderer::Get().Submit(top.c_str(), Vector2(topLeft.x, topLeft.y + h - 1), color, z);
}

static const char* DIGIT_3x5[10][5] =
{
    {"###","# #","# #","# #","###"}, //0
    {" ##","  #","  #","  #","###"}, //1
    {"###","  #","###","#  ","###"}, //2
    {"###","  #","###","  #","###"}, //3
    {"# #","# #","###","  #","  #"}, //4
    {"###","#  ","###","  #","###"}, //5
    {"###","#  ","###","# #","###"}, //6
    {"###","  #","  #","  #","  #"}, //7
    {"###","# #","###","# #","###"}, //8
    {"###","# #","###","  #","###"}, //9
};

static void DrawBigNumber3x5(int value, const Vector2& pos, Color color, int z, int scale = 2)
{
    if (value < 0) value = 0;

    std::string s = std::to_string(value);

    // 한 줄(3x5의 row 하나)을 scale에 맞게 늘려서 Submit
    for (int row = 0; row < 5; ++row)
    {
        std::string line;

        for (char ch : s)
        {
            int d = ch - '0';
            const char* pat = DIGIT_3x5[d][row];

            // 가로 스케일
            for (int i = 0; i < 3; ++i)
                line.append(scale, pat[i]);

            line.append(scale, ' '); // 숫자 간격
        }

        // 세로 스케일: 같은 줄을 scale번 반복 출력
        for (int sy = 0; sy < scale; ++sy)
        {
            Renderer::Get().Submit(
                line.c_str(),
                Vector2(pos.x, pos.y + row * scale + sy),
                color,
                z
            );
        }
    }
}

GameLevel::GameLevel()
{
    // Player 액터 추가.
    AddNewActor(new Player());

    // 적 생성기 추가.
    AddNewActor(new EnemySpawner());

    // Test: 마우스 테스터 추가.
    AddNewActor(new MouseTester());
    quadtree = new QuadTree(Rect(0, 0, Engine::Get().GetWidth(), Engine::Get().GetHeight()), 4, 0, 4);

    //Renderer::Get().SetCameraOffset(cameraOffset);

    LoadMap("map.txt");
}

GameLevel::~GameLevel()
{
    SafeDelete(quadtree);
}

void GameLevel::BeginPlay()
{
    super::BeginPlay();    

    if (isStarted)
        return;
    isStarted = true;

    StartTime();

    PlayerBullet::Prewarm(this, 200);
    Enemy::Prewarm(this, 5000);
    ExpGem::Prewarm(this, 150);

    ProcessAddAndDestroyActors();
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

    //const int fpsInt = (int)(cachedFps + 0.5);

    //const int hudW = 30;
    //const int hudH = 16; // big number(scale=2) + text 줄 고려
    //const int hudX = 1;
    //const int hudY = Engine::Get().GetHeight() - hudH - 1;

    //// 1) HUD 영역 싹 비우기 (배경)
    //ClearRect(Vector2(hudX, hudY), hudW, hudH, Color::Black, 9);

    //// 2) 테두리(선택)
    //DrawBox(Vector2(hudX, hudY), hudW, hudH, Color::White, 10);

    //// 3) 큰 FPS 숫자 (박스 안쪽에)
    //DrawBigNumber3x5(fpsInt, Vector2(hudX + 2, hudY + 2), Color::White, 10, 2);

    //// 4) 작은 텍스트
    //char info[128];
    //sprintf_s(info, 128, "dt: %.4f", deltaTime);
    //Renderer::Get().Submit(info, Vector2(hudX + 2, hudY + hudH - 2), Color::White, 10);



    // Normal Text
    //static char fpsString[128]; 
    sprintf_s(fpsString, 128, "FPS: %.1f  dt: %.4f", cachedFps, deltaTime);

    Renderer::Get().Submit(
        fpsString,
        Vector2(10, Engine::Get().GetHeight() - 1), Color::White, 10
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

void GameLevel::UpdateQuadTreeDebugLines()
{
    
    quadDebugLines.clear();
    quadDebugRects.clear();
    quadDebugRenderStrings.clear();

    if (!quadtree)
        return;

    if (showQuadTreeDebugLines)
    {
        quadDebugLines.emplace_back("[QuadTree Bounds]");
        quadtree->DebugCollectBounds(quadDebugLines, quadDebugMaxDepth, quadDebugOnlyDepth);
    }
    else
    {
        quadDebugLines.clear();
    }

    if (showQuadTreeDebugRects)
    {
        quadtree->DebugCollectRects(quadDebugRects, quadDebugMaxDepth, quadDebugOnlyDepth);
    }
}
void GameLevel::Tick(float deltaTime)
{
    //super::Tick(deltaTime);

    //Debug Active Button
    CheckDebugButton();
    
    ShowStats();



    // 충돌 판정 처리.
    ProcessCollisionPlayerBulletAndEnemy();
    ProcessCollisionPlayerAndEnemyAABB();
    //ProcessCollisionPlayerAndEnemyQuadTree();
    ProcessCollisionPlayerAndExpGemQuadTree();
    ProcessCollisionPlayerAndEnemyBullet();
    ProcessAstarAlgorithmPlayerAndEnemy();


    //Level Up UI
    for (Actor* const actor : actors)
    {
        if (!actor->IsActive())
        {
            continue;
        }

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

    Player* playerForCamera = nullptr;
    for (Actor* const actor : actors)
    {
        if (!actor->IsActive())
        {
            continue;
        }

        if (actor->IsTypeOf<Player>())
        {
            playerForCamera = actor->As<Player>();
            break;
        }
    }

    // Camera Update
    //if (playerForCamera)
    //{
    //    UpdateCamera(*playerForCamera);
    //}

    if (isShowStat)
        PrintFPS(deltaTime);

    sprintf_s(detailString, 128, "Quad Tree Debug Key F1 ~ F4");
    Renderer::Get().Submit(
        detailString,
        Vector2(20, Engine::Get().GetHeight() - 1), Color::Red, 10
    );

    sprintf_s(timeString, 128, "Play Time : %lld",GetSeconds());
    Renderer::Get().Submit(
        timeString,
        Vector2(25, Engine::Get().GetHeight() - 3), Color::Red, 10
    );
    // QuadTree 생성.
    MakeQuadTree();

    UpdateQuadTreeDebugLines();

}

void GameLevel::Draw()
{
    DrawBackground();
    super::Draw();

    if (isPlayerDead)
    {
        // 플레이어 죽음 메시지 Renderer에 제출.
        //Renderer::Get().SubmitWorld("!Dead!", playerDeadPosition);
        Renderer::Get().Submit("!Dead!", playerDeadPosition);

        // 점수 보여주기.
        ShowScore();

        // 화면에 바로 표시.
        Renderer::Get().PresentImmediately();

        // 프로그램 정지.
        Sleep(2000);

        isPlayerDead = false;
        isStarted = false;
        Game::Get().ToggleMenu();
        // 게임 종료.
        //Engine::Get().QuitEngine();
    }

    // 점수 보여주기.
    if(isShowStat)
        ShowScore();

    DrawQuadTreeDebug();
}

void GameLevel::DrawQuadTreeDebug()
{

    if (showQuadTreeDebugRects)
    {
        for (const QuadTree::DebugRect& entry : quadDebugRects)
        {
            DrawDebugRect(entry.bounds, entry.depth);
        }
    }

    if (showQuadTreeDebugLines)
    {
        int y = 2; // exp/hp 바 아래부터 출력
        for (const std::string& line : quadDebugLines)
        {
            Renderer::Get().Submit(line.c_str(), Vector2(0, y), Color::White, 0);
            ++y;
            if (y >= Engine::Get().GetHeight())
            {
                break;
            }
        }
    }
}

void GameLevel::DrawDebugRect(const Rect& rect, int depth)
{
    if (rect.width <= 0 || rect.height <= 0)
    {
        return;
    }

    const int screenW = Engine::Get().GetWidth();
    const int screenH = Engine::Get().GetHeight();

    int left = max(0, rect.x);
    int top = max(0, rect.y);
    int right = min(screenW - 1, rect.x + rect.width - 1);
    int bottom = min(screenH - 1, rect.y + rect.height - 1);

    if (right < left || bottom < top)
    {
        return;
    }

    Color color = Color::White;
    switch (depth & 3)
    {
    case 1: color = Color::Red; break;
    case 2: color = Color::Green; break;
    case 3: color = Color::Blue; break;
    default: color = Color::White; break;
    }

    const int lineLength = right - left + 1;
    if (lineLength <= 0)
    {
        return;
    }

    const int sorting = 0;

    // top line
    {
        std::string line(lineLength, '-');
        line.front() = '+';
        line.back() = '+';
        quadDebugRenderStrings.emplace_back(line);
        Renderer::Get().Submit(quadDebugRenderStrings.back().c_str(), Vector2(left, top), color, sorting);
    }

    // bottom line
    if (bottom != top)
    {
        std::string line(lineLength, '-');
        line.front() = '+';
        line.back() = '+';
        quadDebugRenderStrings.emplace_back(line);
        Renderer::Get().Submit(quadDebugRenderStrings.back().c_str(), Vector2(left, bottom), color, sorting);
    }

    // vertical lines
    if (bottom - top >= 2)
    {
        for (int y = top + 1; y <= bottom - 1; ++y)
        {
            Renderer::Get().Submit("|", Vector2(left, y), color, sorting);
            if (right != left)
            {
                Renderer::Get().Submit("|", Vector2(right, y), color, sorting);
            }
        }
    }
}

void GameLevel::ProcessCollisionPlayerBulletAndEnemy()
{
    // 플레이어 탄약과 적 액터 필터링.
    std::vector<PlayerBullet*> bullets;
    std::vector<Enemy*> enemies;

    // 액터 필터링.
    for (Actor* const actor : actors)
    {
        if (!actor->IsActive())
        {
            continue;
        }

        if (actor->IsTypeOf<PlayerBullet>())
        {
            bullets.emplace_back(actor->As<PlayerBullet>());
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
    for (PlayerBullet* const bullet : bullets)
    {
        if (!bullet->IsActive())
        {
            continue;
        }

        for (Enemy* const enemy : enemies)
        {
            if (!bullet->IsActive())
            {
                break;
            }

            if (!enemy->IsActive())
            {
                continue;
            }

            // AABB 겹침 판정.
            if (bullet->TestIntersect(enemy))
            {
                enemy->OnDamaged();
                bullet->ReleaseToPool();

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
        if (!actor->IsActive())
        {
            continue;
        }

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
        if (!actor->IsActive())
        {
            continue;
        }

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
        //quadtree.
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
        if (!actor->IsActive())
        {
            continue;
        }

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
                    actor->As<ExpGem>()->ReleaseToPool();
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
        if (!actor->IsActive())
        {
            continue;
        }

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
        Vector2(0, Engine::Get().GetHeight() - 1), Color::White, 1
    );

}

void GameLevel::ShowStats()
{
    if (Input::Get().GetKeyDown('1'))
        isShowStat = !isShowStat;
}

void GameLevel::CheckDebugButton()
{
    if (Input::Get().GetKeyDown(VK_F1))
    {
        // QuadTree Bound 활성화, 나머지 비활성화
        PrintNoDebug();
    }
    if (Input::Get().GetKeyDown(VK_F2))
    {
        // QuadTree Text 활성화, 나머지 비활성화
        PrintQuadDebugText();
    }
    if (Input::Get().GetKeyDown(VK_F3))
    {
        // QuadTree Bound 활성화, 나머지 비활성화
        PrintQuadDebugRect();
    }
    if (Input::Get().GetKeyDown(VK_F4))
    {
        // 모두 활성화
        showQuadTreeDebugLines = true;
        showQuadTreeDebugRects = true;
    }
}

void GameLevel::PrintQuadDebugText()
{
    showQuadTreeDebugLines = true;
    showQuadTreeDebugRects = false;
}

void GameLevel::PrintQuadDebugRect()
{
    showQuadTreeDebugLines = false;
    showQuadTreeDebugRects = true;
}

void GameLevel::PrintNoDebug()
{
    showQuadTreeDebugLines = false;
    showQuadTreeDebugRects = false;
}
 
//
//void GameLevel::UpdateCamera(const Player& player)
//{
//    const int screenW = Engine::Get().GetWidth();
//    const int screenH = Engine::Get().GetHeight();
//
//    const int marginX = screenW / 5;
//    const int marginY = screenH / 5;
//
//    int camX = cameraOffset.x;
//    int camY = cameraOffset.y;
//
//    const Vector2 playerPos = player.GetPosition();
//
//    if (playerPos.x - camX < marginX)
//    {
//        camX = playerPos.x - marginX;
//    }
//    else if (playerPos.x - camX > screenW - marginX)
//    {
//        camX = playerPos.x - (screenW - marginX);
//    }
//
//    if (playerPos.y - camY < marginY)
//    {
//        camY = playerPos.y - marginY;
//    }
//    else if (playerPos.y - camY > screenH - marginY)
//    {
//        camY = playerPos.y - (screenH - marginY);
//    }
//
//    cameraOffset = Vector2(camX, camY);
//    Renderer::Get().SetCameraOffset(cameraOffset);
//}

//void GameLevel::DrawBackground()
//{
//    const int screenW = Engine::Get().GetWidth();
//    const int screenH = Engine::Get().GetHeight();
//
//    if (backgroundLines.size() != static_cast<size_t>(screenH))
//    {
//        backgroundLines.assign(static_cast<size_t>(screenH), std::string(screenW, ' '));
//    }
//    else
//    {
//        for (std::string& line : backgroundLines)
//        {
//            if (static_cast<int>(line.size()) != screenW)
//            {
//                line.assign(screenW, ' ');
//            }
//        }
//    }
//
//    for (int y = 0; y < screenH; ++y)
//    {
//        std::string& line = backgroundLines[static_cast<size_t>(y)];
//        for (int x = 0; x < screenW; ++x)
//        {
//            const int worldX = x + cameraOffset.x;
//            const int worldY = y + cameraOffset.y;
//
//            char c = ' ';
//            const int seed = (worldX * 37 + worldY * 17);
//            if (seed % 23 == 0)
//            {
//                c = '`';
//            }
//            else if (seed % 29 == 0)
//            {
//                c = ',';
//            }
//
//            line[static_cast<size_t>(x)] = c;
//        }
//
//        Renderer::Get().Submit(line.c_str(), Vector2(0, y), Color::Green, 0);
//    }
//}

void GameLevel::DrawBackground()
{
    const int screenW = Engine::Get().GetWidth();
    const int screenH = Engine::Get().GetHeight();

    // 매번 할당하지 않도록 멤버 변수로 관리하는 것을 추천합니다.
    if (backgroundLines.size() != static_cast<size_t>(screenH))
    {
        backgroundLines.assign(static_cast<size_t>(screenH), std::string(screenW, ' '));
    }

    for (int y = 0; y < screenH; ++y)
    {
        std::string& line = backgroundLines[static_cast<size_t>(y)];
        const int worldY = y;

        for (int x = 0; x < screenW; ++x)
        {
            const int worldX = x;
            char c = ' '; // 기본 땅

            // 월드 맵 범위 내에 있는지 확인
            if (worldY >= 0 && worldY < (int)worldMap.size() &&
                worldX >= 0 && worldX < (int)worldMap[worldY].size())
            {
                // '1'이면 벽('#'), '0'이면 공백 또는 장식
                if (worldMap[worldY][worldX] == '1')
                {
                    c = '#';
                }
                else
                {
                    // 기존에 사용하시던 배경 장식(`, ,) 로직을 여기에 넣으면 더 예쁩니다.
                    const int seed = (worldX * 37 + worldY * 17);
                    if (seed % 23 == 0) c = '`';
                    else if (seed % 29 == 0) c = ',';
                }
            }
            line[x] = c;
        }
        Renderer::Get().Submit(line.c_str(), Vector2(0, y), Color::Green, 0);
    }
}

void GameLevel::LoadMap(const char* filename)
{
    char path[2048] = {};
    sprintf_s(path, 2048, "../Assets/%s", filename);

    FILE* file = nullptr;
    fopen_s(&file, path, "rt");

    if (!file)
    {
        std::cerr << "Failed to open map file.\n";
        __debugbreak();
        return;
    }

    // 파일 크기 파악
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    // 데이터 읽기
    char* data = new char[fileSize + 1];
    size_t readSize = fread(data, sizeof(char), fileSize, file);
    data[readSize] = '\0';

    // worldMap 초기화
    worldMap.clear();
    worldMap.push_back("");
    int currentRow = 0;

    for (size_t i = 0; i < readSize; ++i)
    {
        char mapCharacter = data[i];

        // 개행 문자 처리 (\r\n 또는 \n 대응)
        if (mapCharacter == '\n')
        {
            worldMap.push_back("");
            currentRow++;
            continue;
        }
        if (mapCharacter == '\r') continue;

        // 단순히 문자를 worldMap 행에 추가
        worldMap[currentRow] += mapCharacter;
    }

    // 마지막 빈 줄 정리
    if (!worldMap.empty() && worldMap.back().empty()) worldMap.pop_back();

    delete[] data;
    fclose(file);
}

void GameLevel::ProcessAstarAlgorithmPlayerAndEnemy()
{
    // 플레이어 탄약과 적 액터 필터링.
    Player* player = nullptr;
    std::vector<Enemy*> enemies;

    // 액터 필터링.
    for (Actor* const actor : actors)
    {
        if (!actor->IsActive())
        {
            continue;
        }

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


