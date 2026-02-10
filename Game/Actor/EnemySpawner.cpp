#include "EnemySpawner.h"
#include "Actor/Enemy.h"
#include "Util/Util.h"
#include "Level/Level.h"
#include "Render/Renderer.h"
#include <vector>
#include <cstring>

// 적 생성할 때 사용할 글자 값.
// 여기에서 static은 private.
static const char* enemyType[] =
{
	";:^:;",
	"zZwZz",
	"oO@Oo",
	"<-=->",
	")qOp(",
};

EnemySpawner::EnemySpawner()
{
	// 적 생성 타이머 설정.
	shotTimer.SetTargetTime(Util::RandomRange(1.0f, 2.0f));
}

void EnemySpawner::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	SpawnEnemy(deltaTime);
}

void EnemySpawner::SpawnEnemy(float deltaTime)
{
	// 타이머 업데이트.
	shotTimer.Tick(deltaTime);

	// 경과 시간 확인.
	if (!shotTimer.IsTimeOut())
	{
		return;
	}

	// 타이머 초기화.
	shotTimer.Reset();

	// 적생성.
	// 적 개수 파악.
	static int length 
		= sizeof(enemyType) / sizeof(enemyType[0]);

	// 랜덤 인덱스.
	int index = Util::Random(0, length - 1);

	// 생성 y 위치.
	//int yPosition = Util::Random(1, 10);

	// 적 생성 요청.
	const Vector2 cam = Renderer::Get().GetCameraOffset();
	const int screenW = Engine::Get().GetWidth();
	const int screenH = Engine::Get().GetHeight();

	const int enemyWidth = static_cast<int>(strlen(enemyType[index]));
	const int enemyHeight = 1;

	const int minX = cam.x;
	const int minY = cam.y;
	const int maxX = cam.x + screenW - enemyWidth - 1;
	const int maxY = cam.y + screenH - enemyHeight - 1;

	const int midX = (minX + maxX) / 2;
	const int midY = (minY + maxY) / 2;

	std::vector<Vector2> positions =
	{
		{minX, minY},   {midX, minY},   {maxX, minY},
		{minX, midY},                 {maxX, midY},
		{minX, maxY},   {midX, maxY},   {maxX, maxY},
	};

	const int posIndex = Util::Random(0, static_cast<int>(positions.size()) - 1);
	Enemy::Acquire(GetOwner(), enemyType[index], positions[posIndex]);
}
