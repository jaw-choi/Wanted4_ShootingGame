#pragma once

#include "Actor/Actor.h"
#include <vector>

namespace Wanted { class Level; }
using namespace Wanted;

class PlayerBullet : public Actor
{
	RTTI_DECLARATIONS(PlayerBullet, Actor)

public:
	PlayerBullet(const Vector2f& position,const Vector2f& dir, float _moveSpeed);
	~PlayerBullet();

	static PlayerBullet* Acquire(Level* owner, const Vector2f& position, const Vector2f& dir, float moveSpeed);
	static void Prewarm(Level* owner, int count);
	void ReleaseToPool();
	void Initialize(const Vector2f& position, const Vector2f& dir, float moveSpeed);

	virtual void Tick(float deltaTime) override;

private:

	// 이동 속도.
	// 초당 몇칸 이동하는지를 나타냄.
    float moveSpeed = 0.f;

	// 위치 갱신을 할 때 소수점처리를 위한 변수.
	//float yPosition = 0.0f;
    Vector2f currPos;
    Vector2f dir;

	bool inPool = false;

	static std::vector<PlayerBullet*> pool;
};

