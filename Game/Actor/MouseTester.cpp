#include "MouseTester.h"
#include "Core/Input.h"
#include "Render/Renderer.h"
#include "Engine/Engine.h"
#include <iostream>

#include <crtdbg.h>
#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif

MouseTester::MouseTester()
	: super(" ", Vector2(Engine::Get().GetWidth() / 2, Engine::Get().GetHeight() / 2))
{
}

void MouseTester::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 마우스 입력 테스트.
	//if (Input::Get().GetMouseButton(0))
	//{
	//	Vector2 mousePosition = Input::Get().MousePosition();
	//	char buffer[256] = {};
	//	sprintf_s(
	//		buffer, 
	//		256, 
	//		"cursor: (%d, %d)", 
	//		mousePosition.x, 
	//		mousePosition.y
	//	);

	//	ChangeImage(buffer);
	//}
}

