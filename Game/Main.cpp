#include <iostream>
#include "Engine/Engine.h"
#include "Level/GameLevel.h"
#include "Level/MenuLevel.h"
#include "Game/Game.h"

using namespace Wanted;

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//Engine engine;
	Game game;
	game.Run();
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	return 0;
}