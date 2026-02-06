#include "EnemyBullet.h"
#include "Engine/Engine.h"

EnemyBullet::EnemyBullet(
	const Vector2& position, 
	float moveSpeed)
	: super("#", position, Color::Red),
	moveSpeed(moveSpeed), 
	yPosition(static_cast<float>(position.y))
{
	//sortingOrder = 5;
}

void EnemyBullet::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// Todo: 임시 폐쇄 - 나중에 보스 몹이 쓸 수도
	// y위치 업데이트.
	//yPosition = yPosition + moveSpeed * deltaTime;

	//// 좌표 검사 (화면 아래 끝 지점을 벗어났는지 확인).
	//if (yPosition >= Engine::Get().GetHeight())
	//{
	//	Destroy();
	//	return;
	//}

	//// 위치 설정.
	//SetPosition(
	//	Vector2(position.x, static_cast<int>(yPosition))
	//);
}
