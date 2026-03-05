#pragma once

#include "Level/Level.h"
#include "Math/Vector2.h"
#include "Math/QuadTree.h"
#include <string>
#include <vector>
#include <chrono>

using namespace Wanted;

class Player;

class GameLevel : public Level
{
	// RTTI 등록.
	RTTI_DECLARATIONS(GameLevel, Level)


public:
	GameLevel();
	~GameLevel();

	QuadTree* GetQuadtree() const { return quadtree; }

	inline void ShowLevelUpUI() { isLevelUpUIVisible  = true; }
	inline void HideLevelUpUI() { isLevelUpUIVisible = false; }
	inline bool GetIsShowStat() { return isShowStat; }
private:

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	//Quadtree
	void MakeQuadTree();
	void UpdateQuadTreeDebugLines();
	void DrawQuadTreeDebug();
	void DrawDebugRect(const Rect& rect, int depth);

	void PrintFPS(float deltaTime);

	// 충돌 판정 처리 함수.
	void ProcessCollisionPlayerBulletAndEnemy();
	void ProcessCollisionPlayerAndEnemyAABB();
	void ProcessCollisionPlayerAndEnemyQuadTree();
	void ProcessCollisionPlayerAndExpGemQuadTree();
	void ProcessCollisionPlayerAndEnemyBullet();
	void ProcessAstarAlgorithmPlayerAndEnemy();

	// 점수 보여주는 함수.
	void ShowScore();

	// player 스탯 보여주는 함수
	void ShowStats();

	// Debug Button Check
	void CheckDebugButton();
	void PrintQuadDebugText();
	void PrintQuadDebugRect();
	void PrintNoDebug();

	// 카메라 및 배경
	//void UpdateCamera(const Player& player);
	void DrawBackground();
private:
    void StartTime()
    {
	start_ = std::chrono::steady_clock::now();
    }

    long long GetSeconds() const
    {
	using namespace std::chrono;
	return duration_cast<seconds>(steady_clock::now() - start_).count();
    }

private:
    std::chrono::steady_clock::time_point start_{ std::chrono::steady_clock::now() };
private:
	//QuadTree
	QuadTree* quadtree;

	// 점수 변수.
	int score = 0;

	// 플레이어가 죽었는지 확인.
	bool isPlayerDead = false;

	// 플레이어가 죽은 위치 (Draw에서 처리하기 위해 Tick에서 저장).
	Vector2 playerDeadPosition;

	// 점수 문자열.
	char scoreString[128] = {};
	char fpsString[128] = {};
	char detailString[128] = {};
	char timeString[128] = {};

	bool isShowStat = true;
	bool isLevelUpUIVisible = false;

	Actor* levelUpUIInstance = nullptr;

	// QuadTree 디버그 출력
	bool showQuadTreeDebugLines = false;
	bool showQuadTreeDebugRects = false;

	int quadDebugMaxDepth = -1;
	int quadDebugOnlyDepth = -1;

	std::vector<std::string> quadDebugLines;

	std::vector<QuadTree::DebugRect> quadDebugRects;
	std::vector<std::string> quadDebugRenderStrings;

	Vector2 cameraOffset = Vector2::Zero;
	std::vector<std::string> backgroundLines;

        bool isStarted = false;
};
