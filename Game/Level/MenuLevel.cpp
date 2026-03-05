#include "MenuLevel.h"
#include "Game/Game.h"
#include "Core/Input.h"
#include "Util/Util.h"
#include "Render/Renderer.h"
#include <iostream>

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

 //   if (Input::Get().GetKeyDown(VK_ESCAPE))
 //   {
	//// 메뉴 토글.
	//Game::Get().ToggleMenu();

	//// 인덱스 초기화.
	//currentIndex = 0;
 //   }
}

void MenuLevel::Draw()
{
    // 메뉴 제목 출력.
    //Util::SetConsolePosition(Vector2::Zero);
    //Util::SetConsoleTextColor(Color::White);
    //
    //// 텍스트 출력.
    //std::cout << "Sokoban Game\n\n";

    int nextLine = Renderer::Get().SubmitFromFile("../Assets/title.txt", {Engine::Get().GetWidth() / 4 + 150, Engine::Get().GetHeight() / 4} );
    int prev = Engine::Get().GetHeight() / 16 + nextLine;
    int padding = 0;
    // 메뉴 아이템 출력.
    for (int ix = 0; ix < static_cast<int>(items.size()); ++ix)
    {
	// 아이템 색상 확인 (선택됐는지 여부).
	if (ix == 1)
	    padding = 10;
	Color textColor =
	    (ix == currentIndex) ? selectedColor : unselectedColor;
	nextLine = Renderer::Get().SubmitFromFile(
	    items[ix]->text,
	    Vector2(Engine::Get().GetWidth() / 2 - 80 + padding ,prev+50),
	    textColor
	);
	prev = nextLine;

	// 색상 설정.
	//Util::SetConsoleTextColor(textColor);

	// 텍스트 출력.
	//std::cout << items[ix]->text << "\n";
    }
}

