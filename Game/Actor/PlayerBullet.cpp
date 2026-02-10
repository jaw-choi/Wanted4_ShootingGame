#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(const Vector2f& position, const Vector2f& dir, float _moveSpeed)
	: super("@", Vector2((int)position.x,(int)position.y), Color::Blue),
    currPos(position), dir(dir.Normalized()), moveSpeed(_moveSpeed)
{
}

PlayerBullet::~PlayerBullet()
{
}

void PlayerBullet::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	
	// 위로 이동 처리. 초단위로 이동.
	//yPosition -= moveSpeed * deltaTime;
	//x,y position 계산하기


	// 좌표 검사.
	if (!IsOnScreen())
	{
		// 삭제 처리.
		Destroy();
		return;
	}

	// 위치 갱신.
	// 이렇게 하니 movespeed 안먹음
	//Vector2f pos(GetPosition());
	//pos = pos + dir * moveSpeed * deltaTime;

	currPos = currPos + dir * moveSpeed * deltaTime;
	SetPosition(Vector2(
	    static_cast<int>(currPos.x),
	    static_cast<int>(currPos.y)
	));
	
	// 액터의 위치로 변환.
	//Vector2 newPosition = GetPosition();
	//newPosition.y = static_cast<int>(yPosition);
	
	// 위치 갱신.
	//SetPosition(newPosition);
}

