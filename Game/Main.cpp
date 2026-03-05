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

void GenerateMapFile(const std::string& filename, int width, int height) {
    std::ofstream mapFile(filename);
    if (!mapFile.is_open()) return;

    // 맵 데이터를 '0'으로 초기화
    std::vector<std::vector<char>> mapData(height, std::vector<char>(width, '0'));

    srand(static_cast<unsigned int>(time(NULL)));

    // 1. 테두리 처리
    for (int y = 0; y < height; ++y) {
        mapData[y][0] = '1';
        mapData[y][width - 1] = '1';
    }
    for (int x = 0; x < width; ++x) {
        mapData[0][x] = '1';
        mapData[height - 1][x] = '1';
    }

    // 2. 대형 박스 10개 생성
    int boxCount = 10;
    for (int i = 0; i < boxCount; ++i) {
        // 최소 크기 100x200 ~ 최대 크기 (예: 150x250) 설정
        int boxW = rand() % 50 + 20; // 너비: 100 ~ 149
        int boxH = rand() % 50 + 10; // 높이: 200 ~ 249

        // 맵 범위를 벗어나지 않도록 시작 위치 결정
        // (테두리 1칸을 제외한 위치)
        int startX = rand() % (width - boxW - 2) + 1;
        int startY = rand() % (height - boxH - 2) + 1;

        // 박스 그리기
        for (int y = startY; y < startY + boxH; ++y) {
            for (int x = startX; x < startX + boxW; ++x) {
                mapData[y][x] = '1';
            }
        }
    }

    // 3. 파일 쓰기
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            mapFile << mapData[y][x];
        }
        mapFile << "\n";
    }

    mapFile.close();
    std::cout << "Large obstacles generated: " << filename << std::endl;
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//Engine engine;
	Game game;
        GenerateMapFile("../Assets/map.txt", game.GetWidth(), game.GetHeight());
	game.Run();
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	return 0;
}