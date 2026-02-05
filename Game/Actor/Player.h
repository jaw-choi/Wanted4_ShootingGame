#pragma once

#include "Actor/Actor.h"
#include "Util/Timer.h"

using namespace Wanted;

class Player : public Actor
{
	// 발사 모드.
	enum class FireMode
	{
		None = -1,
		OneShot,
		Repeat
	};

	RTTI_DECLARATIONS(Player, Actor)

public:
	Player();
	~Player();

private:
	virtual void Tick(float deltaTime) override;

	// 오른쪽으로 이동하는 함수.
	void MoveRight();
	
	// 왼쪽으로 이동하는 함수.
	void MoveLeft();

	// 아래로 이동하는 함수.
	void MoveDown();

	// 위로 이동하는 함수.
	void MoveUp();

	// 탄약 발사 함수.
	void Fire();

	// 연속 발사 함수.
	void FireInterval();

	// 연속 이동 함수.
	void MoveUpInterval();
	void MoveDownInterval();
	void MoveLeftInterval();
	void MoveRightInterval();

	// 발사 가능여부 확인 함수.
	bool CanShoot() const;

	// Y축 이동 가능여부 확인 함수.
	bool CanMoveY() const;
	bool CanMoveX() const;

private:
	// 발사 모드.
	FireMode fireMode = FireMode::None;

	// 타이머 변수.
	Timer timer;

	// 타이머 변수.
	Timer yPostimer;
	Timer xPostimer;

	// 연사 시간 간격.
	float fireInterval = 0.2f;

	float moveXInterval = 0.2f;
	float moveYInterval = 0.2f;
};
