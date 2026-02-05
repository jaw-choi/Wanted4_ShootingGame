#pragma once

#include "Actor/Actor.h"
#include "Util/Timer.h"

using namespace Wanted;

class Enemy : public Actor
{
	RTTI_DECLARATIONS(Enemy, Actor)

	// 이동 방향 열거형.
	enum class MoveDirection
	{
		None = -1,
		Left,
		Right,
		Up,
		Down
	};

public:
	Enemy(const char* image = "(oOo)");
	~Enemy();

	// Tick.
	virtual void Tick(float deltaTime) override;

	// 대미지 받았을 때 처리할 함수.
	void OnDamaged();

	void MoveTo(const Actor& target);
private:
	// 이동 방향 열거형.
	MoveDirection direction = MoveDirection::None;

	// 좌우 이동 처리를 위한 변수.

	float moveSpeed = 6.f;

	Vector2f dir;
	Vector2f currPos;

	// 발사 타이머.
	Timer timer;
};
