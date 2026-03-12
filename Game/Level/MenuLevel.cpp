#include "MenuLevel.h"
#include "Game/Game.h"
#include "Core/Input.h"
#include "Util/Util.h"
#include "Render/Renderer.h"
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <Windows.h>

std::vector<std::vector<int>> grid = {
    {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row  0
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row  1
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row  2
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row  3
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row  4
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row  5
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row  6
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row  7
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row  8
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row  9
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row 10
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row 11
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row 12
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row 13
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row 14
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row 15
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row 16
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row 17
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // row 18
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1},  // row 19
    
};

static void FindStartAndGoalNode(
    const std::vector<std::vector<int>>& grid,
    Node*& outStartNode,
    Node*& outGoalNode
);

MenuLevel::MenuLevel()
{
    // 메뉴 아이템 생성.
    items.emplace_back(new MenuItem(
	"../Assets/startGame.txt",
	[]()
	{
	    // 메뉴 토글 함수 호출.
	    Game::Get().ToggleMenu();
	}
    ));

    items.emplace_back(new MenuItem(
	"../Assets/quitGame.txt",
	[]()
	{
	    // 게임 종료.
	    Game::Get().QuitEngine();
	}
    ));
}

MenuLevel::~MenuLevel()
{
    // 메뉴 아이템 제거.
    for (MenuItem*& item : items)
    {
	delete item;
	item = nullptr;
    }

    // 배열 초기화.
    items.clear();
}

void MenuLevel::Tick(float deltaTime)
{
    super::Tick(deltaTime);

    // 입력 처리 (방향키 위/아래키, 엔터키, ESC키).
    // 배열 길이.
    static int length = static_cast<int>(items.size());
    if (Input::Get().GetKeyDown(VK_UP))
    {
	// 인덱스 돌리기 (위 방향으로).
	currentIndex = (currentIndex - 1 + length) % length;
    }

    if (Input::Get().GetKeyDown(VK_DOWN))
    {
	// 인덱스 돌리기.
	currentIndex = (currentIndex + 1) % length;
    }

    if (Input::Get().GetKeyDown(VK_RETURN))
    {
	// 메뉴 아이템이 저장한 함수 포인터 호출.
	items[currentIndex]->onSelected();
    }

    if (Input::Get().GetKeyDown(VK_ESCAPE))
    {
	// 메뉴 토글.
	Game::Get().ToggleMenu();

	// 인덱스 초기화.
	currentIndex = 0;
    }

    InitAStarIfNeeded();
    if (!exploredNodes.empty() && exploreRevealCount < exploredNodes.size())
    {
        exploreReveal += deltaTime * exploreRevealSpeed;
        size_t count = static_cast<size_t>(exploreReveal);
        if (count > exploredNodes.size())
        {
            count = exploredNodes.size();
        }
        exploreRevealCount = count;
    }
    else if (!finalPath.empty())
    {
        pathReveal += deltaTime * pathRevealSpeed;
        size_t count = static_cast<size_t>(pathReveal);
        if (count > finalPath.size())
        {
            count = finalPath.size();
        }
        pathRevealCount = count;
    }
}


void MenuLevel::Draw()
{
    // 메뉴 제목 출력.
    //Util::SetConsolePosition(Vector2::Zero);
    //Util::SetConsoleTextColor(Color::White);
    //
    //// 텍스트 출력.
    //std::cout << "Sokoban Game\n\n";

    // title 출력
    const Vector2 titlePos(Engine::Get().GetWidth() / 4, Engine::Get().GetHeight() / 4);
    //int nextLine = Renderer::Get().SubmitFromFile("../Assets/title.txt", titlePos, Color::White, 5);
    int nextLine = 52;// Renderer::Get().SubmitFromFile("../Assets/title.txt", titlePos, Color::White, 5);
 //   //Title 효과 출력
 //   // 커서 감추기.
 //   CONSOLE_CURSOR_INFO info = {};
 //   info.dwSize = 1;
 //   info.bVisible = FALSE;
 //   SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

 //   // 시작 위치 및 목표 위치.
 //   Node* startNode = nullptr;
 //   Node* goalNode = nullptr;
 //   
 //   // 시작/목표 위치 검색.
 //   FindStartAndGoalNode(
	//grid, startNode, goalNode
 //   );

 //   
 //   // 길찾기.
 //   AStar aStar;
 //   
 //   // 경로 탐색.
 //   auto path = aStar.FindPath(startNode, goalNode, grid);

 //   // 성공한 경우.
 //   if (!path.empty())
 //   {
	//std::cout << "\n경로 탐색 성공.\n";
	//aStar.DisplayGridWithPath(grid, path);
 //   }
 //   // 실패한 경우.
 //   else
 //   {
	//std::cout << "\n경로 탐색 실패.\n";
 //   }

 //   // 목표 노드 메모리 해제.
 //   SafeDelete(goalNode);

 //   // 콘솔 정리(넘어감).
 //   COORD coord = { 0, 30 };
 //   SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);




    InitAStarIfNeeded();

    aStar.SetOrigin(titlePos);
    if (gridRenderString.empty())
    {
        BuildGridRenderString();
    }
    Renderer::Get().Submit(gridRenderString.c_str(), titlePos, Color::White, 0);

    for (size_t i = 0; i < exploreRevealCount; ++i)
    {
        const Vector2& node = exploredNodes[i];
        Renderer::Get().Submit("+",
            Vector2(titlePos.x + node.x * 2, titlePos.y + node.y),
            Color::Green, 1);
    }

    if (!finalPath.empty() && exploreRevealCount >= exploredNodes.size())
    {
	aStar.DisplayGridWithPath(gridWork, finalPath, pathRevealCount);
    }

    // 메뉴 아이템 출력.
    int prev = nextLine;
    int padding = -30;
    for (int ix = 0; ix < static_cast<int>(items.size()); ++ix)
    {
	// 아이템 색상 확인 (선택됐는지 여부).

	Color textColor =
	    (ix == currentIndex) ? selectedColor : unselectedColor;
	if (ix == 1)
	    padding += 3;
	nextLine = Renderer::Get().SubmitFromFile(
	    items[ix]->text,
	    Vector2(Engine::Get().GetWidth() / 2 + padding, abs(Engine::Get().GetHeight() / 2 - prev) + prev),
	    textColor
	);
	prev = nextLine;
    }
}

void MenuLevel::InitAStarIfNeeded()
{
    if (astarInitialized)
    {
	return;
    }

    gridWork = grid;

    Node* startNode = nullptr;
    Node* goalNode = nullptr;
    FindStartAndGoalNode(gridWork, startNode, goalNode);

    if (startNode && goalNode)
    {
	finalPath = aStar.FindPath(startNode, goalNode, gridWork);
        exploredNodes = aStar.GetExplored();
	pathReveal = 0.0f;
	pathRevealCount = 0;
        exploreReveal = 0.0f;
        exploreRevealCount = 0;
	BuildGridRenderString();
	astarInitialized = true;
    }
}

void MenuLevel::BuildGridRenderString()
{
    gridRenderString.clear();
    if (gridWork.empty() || gridWork[0].empty())
    {
        return;
    }

    const int rows = static_cast<int>(gridWork.size());
    const int cols = static_cast<int>(gridWork[0].size());
    gridRenderString.reserve(static_cast<size_t>(rows) * (cols * 2 + 1));

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            const int v = gridWork[y][x];
            char c = '0';
            if (v == 1) c = '1';
            else if (v == 2) c = 'S';
            else if (v == 3) c = 'G';
            else if (v == 5) c = '0';
            gridRenderString.push_back(c);
            gridRenderString.push_back(' ');
        }
        gridRenderString.push_back('\n');
    }
}

void FindStartAndGoalNode(
    const std::vector<std::vector<int>>& grid,
    Node*& outStartNode,
    Node*& outGoalNode)
{
    // 빈 그리드는 검색 불가.
    if (grid.empty() || grid[0].empty())
    {
	return;
    }

    for (int y = 0; y < static_cast<int>(grid.size()); ++y)
    {
	// 다 찾았으면 종료.
	if (outStartNode && outGoalNode)
	{
	    break;
	}

	for (int x = 0; x < static_cast<int>(grid[0].size()); ++x)
	{
	    // 다 찾았으면 종료.
	    if (outStartNode && outGoalNode)
	    {
		break;
	    }

	    // 시작 노드.
	    if (grid[y][x] == 2)
	    {
		outStartNode = new Node(x, y);
		continue;
	    }

	    // 목표 노드.
	    if (grid[y][x] == 3)
	    {
		outGoalNode = new Node(x, y);
		continue;
	    }
	}
    }
}



AStar::AStar()
{
}

AStar::~AStar()
{
    Reset();
}

std::vector<Node*> AStar::FindPath(
    Node* startNode, Node* goalNode,
    std::vector<std::vector<int>>& grid)
{
    Reset();

    // 시작/목표 노드 저장.
    this->startNode = startNode;
    this->goalNode = goalNode;

    // 예외처리.
    if (!this->startNode || !this->goalNode
	|| grid.empty() || grid[0].empty())
    {
	// 빈 경로 반환 (오류).
	//return std::vector<Node*>();
	state = State::Failed;
	return { };
    }

    // 시작 노드를 열린리스트에 추가 및 탐색 시작.
    openList.emplace_back(this->startNode);

    // 대각선 비용 상수.
    const float diagonalcost = 1.41421345f;

    // 비용 계산에 사용할 변수 값 설정.
    std::vector<Direction> directions =
    {
	// 하상우좌 이동.
	{ 0, 1, 1.0f }, { 0, -1, 1.0f }, {1, 0, 1.0f }, {-1, 0, 1.0f},
	// 대각선 이동.
	{ 1, 1, diagonalcost }, { 1, -1, diagonalcost },
	{ -1, 1, diagonalcost }, { -1, -1, diagonalcost },
    };

    // 탐색 가능한 위치가 있으면 계속 진행.
    while (!openList.empty())
    {
	// fCost가 가장 낮은 노드를 현재 노드로 설정.
	Node* currentNode = FindLowestFCostNode();

	// 현재 노드가 목표 노드인지 확인.
	if (IsDestination(currentNode))
	{
	    // 여기에 goalNode를 메모리 정리용도로 목록에 추가 가능.

	    // 경로 반환 후 종료.
	    path = ConstructPath(currentNode);
	    state = State::Found;
	    return path;
	}

	// 방문 처리를 위해 열린 리스트에서 제거.
	for (auto iterator = openList.begin();
	    iterator != openList.end();
	    ++iterator)
	{
	    // iterator 위치의 노드가 currentNode인지 확인.
	    if ((*iterator) == currentNode)
	    {
		openList.erase(iterator);
		break;
	    }
	}

	// 현재 노드를 방문 노드에 추가.
	closedList.emplace_back(currentNode);


	// 이웃 노드 방문(탐색).
	for (const Direction& direction : directions)
	{
	    // 다음에 이동할 위치(이웃 노드의 위치).
	    int newX = currentNode->position.x + direction.x;
	    int newY = currentNode->position.y + direction.y;

	    // 유효성 검증 (새 위치가 이동 가능한지 확인).
	    if (!IsInRange(newX, newY, grid))
	    {
		continue;
	    }

	    // 새 위치가 이동 가능한 곳인지 확인.
	    // 장애물(못가는곳) = 1.
	    if (grid[newY][newX] == 1)
	    {
		continue;
	    }

	    // 현재 노드를 기준으로 새 gCost 계산.
	    float newGCost = currentNode->gCost + direction.cost;

	    // 갈 수는 있지만, 이미 방문한 곳인지 확인.
	    if (HasVisited(newX, newY, newGCost))
	    {
		continue;
	    }

	    // 방문을 위한 이웃 노드 생성.
	    Node* neighborNode = new Node(newX, newY, currentNode);
	    // 비용 계산.
	    neighborNode->gCost = newGCost;
	    neighborNode->hCost = CalculateHeuristic(
		neighborNode, this->goalNode
	    );
	    neighborNode->fCost = neighborNode->gCost + neighborNode->hCost;

	    // 이웃 노드가 열린 리스트에 있는지 확인.
	    Node* openListNode = nullptr;
	    for (Node* const node : openList)
	    {
		// 위치만 비교해서 열린 리스트에 넣을지 여부 확인.
		if (*node == *neighborNode)
		{
		    openListNode = node;
		    break;
		}
	    }

	    // 이웃 노드가 열린 리스트에 있으면 더 좋은 비용일 때만 처리.
	    if (openListNode)
	    {
		// 비용 확인.
		if (neighborNode->gCost < openListNode->gCost
		    || neighborNode->fCost < openListNode->fCost)
		{
		    // 부모 노드 업데이트.
		    openListNode->parentNode = neighborNode->parentNode;
		    // 비용 업데이트.
		    openListNode->gCost = neighborNode->gCost;
		    openListNode->hCost = neighborNode->hCost;
		    openListNode->fCost = neighborNode->fCost;
		}

		// 임시 노드 메모리 정리.
		SafeDelete(neighborNode);
		continue;
	    }

	    // 방문할 목록에 추가.
	    // 이 노드가 이동 가능한지 확인
	    // 이동 가능한 곳 = 0.
	    if (grid[newY][newX] == 0)
	    {
		// 시각화를 위해 사용 안하는 값 정해서 설정.
		grid[newY][newX] = 5;
                exploredOrder.emplace_back(newX, newY);
	    }

	    // 열린 리스트에 추가.
	    openList.emplace_back(neighborNode);

	    // 즉시 출력/대기 로직 제거 (Tick에서 단계 진행)
	}
    }

    state = State::Failed;
    return  { };
}

void AStar::Begin(Node* startNode, Node* goalNode, std::vector<std::vector<int>>* grid)
{
    Reset();

    this->startNode = startNode;
    this->goalNode = goalNode;
    this->gridPtr = grid;
    this->path.clear();
    this->state = State::Searching;

    if (!this->startNode || !this->goalNode || !gridPtr || gridPtr->empty() || (*gridPtr)[0].empty())
    {
	this->state = State::Failed;
	return;
    }

    startNode->gCost = 0.0f;
    startNode->hCost = CalculateHeuristic(startNode, goalNode);
    startNode->fCost = startNode->gCost + startNode->hCost;

    openList.emplace_back(this->startNode);
}

AStar::State AStar::Step()
{
    if (state != State::Searching)
    {
	return state;
    }

    if (openList.empty() || !gridPtr)
    {
	state = State::Failed;
	return state;
    }

    std::vector<std::vector<int>>& grid = *gridPtr;

    Node* currentNode = FindLowestFCostNode();
    if (!currentNode)
    {
	state = State::Failed;
	return state;
    }

    if (IsDestination(currentNode))
    {
	path = ConstructPath(currentNode);
	state = State::Found;
	return state;
    }

    for (auto iterator = openList.begin(); iterator != openList.end(); ++iterator)
    {
	if ((*iterator) == currentNode)
	{
	    openList.erase(iterator);
	    break;
	}
    }
    closedList.emplace_back(currentNode);

    const float diagonalcost = 1.41421345f;
    std::vector<Direction> directions =
    {
	{ 0, 1, 1.0f }, { 0, -1, 1.0f }, {1, 0, 1.0f }, {-1, 0, 1.0f},
	{ 1, 1, diagonalcost }, { 1, -1, diagonalcost },
	{ -1, 1, diagonalcost }, { -1, -1, diagonalcost },
    };

    for (const Direction& direction : directions)
    {
	int newX = currentNode->position.x + direction.x;
	int newY = currentNode->position.y + direction.y;

	if (!IsInRange(newX, newY, grid))
	{
	    continue;
	}

	if (grid[newY][newX] == 1)
	{
	    continue;
	}

	float newGCost = currentNode->gCost + direction.cost;
	if (HasVisited(newX, newY, newGCost))
	{
	    continue;
	}

	Node* neighborNode = new Node(newX, newY, currentNode);
	neighborNode->gCost = newGCost;
	neighborNode->hCost = CalculateHeuristic(neighborNode, this->goalNode);
	neighborNode->fCost = neighborNode->gCost + neighborNode->hCost;

	Node* openListNode = nullptr;
	for (Node* const node : openList)
	{
	    if (*node == *neighborNode)
	    {
		openListNode = node;
		break;
	    }
	}

	if (openListNode)
	{
	    if (neighborNode->gCost < openListNode->gCost
		|| neighborNode->fCost < openListNode->fCost)
	    {
		openListNode->parentNode = neighborNode->parentNode;
		openListNode->gCost = neighborNode->gCost;
		openListNode->hCost = neighborNode->hCost;
		openListNode->fCost = neighborNode->fCost;
	    }

	    SafeDelete(neighborNode);
	    continue;
	}

	if (grid[newY][newX] == 0)
	{
	    grid[newY][newX] = 5;
	}

	openList.emplace_back(neighborNode);
    }

    return state;
}

void AStar::DisplayGridWithPath(
    std::vector<std::vector<int>>& grid,
    const std::vector<Node*>& path,
    size_t maxNodes)
{
    // 경로 출력.
    auto StableCharString = [](char c) -> const char*
    {
        static char table[256][2];
        static bool initialized = false;
        if (!initialized)
        {
            for (int i = 0; i < 256; ++i)
            {
                table[i][0] = static_cast<char>(i);
                table[i][1] = '\0';
            }
            initialized = true;
        }
        return table[static_cast<unsigned char>(c)];
    };

    const char* label = "CONSOLE SURVIVOR";
    const size_t labelLen = strlen(label);
    size_t labelIndex = 0;
    const size_t limit = min(maxNodes, path.size());
    for (size_t i = 0; i < limit; ++i)
    {
        Node* const node = path[i];
	char ch = '*';
	if (labelLen > 0 && labelIndex < labelLen)
	{
	    ch = label[labelIndex++];
	}
	Renderer::Get().Submit(
	    StableCharString(ch),
	    Vector2(renderOrigin.x + node->position.x * 2, renderOrigin.y + node->position.y),
	    Color::Yellow,
	    2
	);
    }
}

std::vector<Node*> AStar::ConstructPath(Node* goalNode)
{
    // 경로를 저장할 배열 선언.
    std::vector<Node*> path;

    // 역추적하면서 path에 채우기.
    Node* currentNode = goalNode;
    while (currentNode)
    {
	path.emplace_back(currentNode);
	currentNode = currentNode->parentNode;
    }

    // 이렇게 얻은 결과는 순서가 거꾸로.
    // 그래서 거꾸로 다시 정렬이 필요함.
    std::reverse(path.begin(), path.end());
    return path;
}

float AStar::CalculateHeuristic(Node* currentNode, Node* goalNode)
{
    // 추정 비용 계산 함수.
    // 고민해볼 계산 방식.
    // 현재노드에서 목표 노드까지의 비용 계산.
    // 단순 거리를 휴리스틱 비용으로 계산.
    Position diff = *currentNode - *goalNode;
    return static_cast<float>(
	std::sqrt(diff.x * diff.x + diff.y * diff.y)
	);
}

bool AStar::IsInRange(
    int x, int y, const std::vector<std::vector<int>>& grid)
{
    // 예외처리.
    if (grid.empty() || grid[0].empty())
    {
	return false;
    }

    // x, y 범위가 벗어났는지 확인.
    if (x < 0 || x >= static_cast<int>(grid[0].size())
	|| y < 0 || y >= static_cast<int>(grid.size()))
    {
	return false;
    }

    // 벗어나지 않았으면 true 반환.
    return true;
}

bool AStar::HasVisited(int x, int y, float gCost)
{
    // 열린 리스트에 이미 같은 위치가 있고,
    // 기존 위치가 비용이 더 낮으면 방문했다고 판단 (중복 방지).
    for (Node* const node : openList)
    {
	if (node->position.x == x && node->position.y == y
	    && gCost >= node->gCost)
	{
	    return true;
	}
    }

    // 닫힌 리스트에 이미 갖은 위치가 있고, 
    // 비용이 더 낮으면 방문했다고 판단.
    for (Node* const node : closedList)
    {
	if (node->position.x == x && node->position.y == y
	    /*&& gCost >= node->gCost*/)
	{
	    return true;
	}
    }

    return false;
}

bool AStar::IsDestination(const Node* const node)
{
    // 두 노드가 같은지 비교.
    return *node == *goalNode;
}

void AStar::DisplayGrid(std::vector<std::vector<int>>& grid)
{
    // 맵 출력.
    for (int y = 0; y < static_cast<int>(grid.size()); ++y)
    {
	for (int x = 0; x < static_cast<int>(grid[0].size()); ++x)
	{
	    // 시작 위치 = 2.
	    if (grid[y][x] == 2)
	    {
		Renderer::Get().Submit("S", Vector2(renderOrigin.x + x * 2, renderOrigin.y + y), Color::Red, 1);
		continue;
	    }

	    // 목표 위치 = 3.
	    if (grid[y][x] == 3)
	    {
		Renderer::Get().Submit("G", Vector2(renderOrigin.x + x * 2, renderOrigin.y + y), Color::Red, 1);
		continue;
	    }

	    // 장애물 = 1.
	    if (grid[y][x] == 1)
	    {
		Renderer::Get().Submit("1", Vector2(renderOrigin.x + x * 2, renderOrigin.y + y), Color::White, 1);
		continue;
	    }

	    // 경로 = 5.
	    if (grid[y][x] == 5)
	    {
		Renderer::Get().Submit("+", Vector2(renderOrigin.x + x * 2, renderOrigin.y + y), Color::Green, 1);
		continue;
	    }

	    // 빈 공간 = 0.
	    if (grid[y][x] == 0)
	    {
		Renderer::Get().Submit("0", Vector2(renderOrigin.x + x * 2, renderOrigin.y + y), Color::White, 1);
	    }
	}
    }
}

void AStar::Reset()
{
    std::unordered_set<Node*> toDelete;
    for (Node* node : openList) toDelete.insert(node);
    for (Node* node : closedList) toDelete.insert(node);
    if (startNode) toDelete.insert(startNode);
    if (goalNode) toDelete.insert(goalNode);

    for (Node* node : toDelete)
    {
	SafeDelete(node);
    }

    openList.clear();
    closedList.clear();
    path.clear();
    exploredOrder.clear();
    startNode = nullptr;
    goalNode = nullptr;
    gridPtr = nullptr;
    state = State::Idle;
}

Node* AStar::FindLowestFCostNode() const
{
    if (openList.empty())
    {
	return nullptr;
    }

    Node* lowestNode = openList[0];
    for (Node* const node : openList)
    {
	if (node->fCost < lowestNode->fCost)
	{
	    lowestNode = node;
	}
    }

    return lowestNode;
}
