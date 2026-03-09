#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <stack>
#include <algorithm>
#include <utility>
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

    // 3x3 캐릭터 기준으로 통로 폭은 4 이상 확보
    const int border = 1;
    const int corridor = 9; // 통로 폭 (>= 4)
    const int wall = 1;     // 벽 두께
    const int step = corridor + wall;

    // 맵 데이터를 벽으로 초기화
    std::vector<std::vector<char>> mapData(height, std::vector<char>(width, '1'));

    const int usableW = width - border * 2;
    const int usableH = height - border * 2;

    const int cellsX = (usableW + wall) / step;
    const int cellsY = (usableH + wall) / step;

    if (cellsX <= 0 || cellsY <= 0)
    {
        mapFile.close();
        return;
    }

    auto carveCell = [&](int cx, int cy)
    {
        const int startX = border + cx * step;
        const int startY = border + cy * step;
        for (int y = startY; y < startY + corridor && y < height - border; ++y)
        {
            for (int x = startX; x < startX + corridor && x < width - border; ++x)
            {
                mapData[y][x] = '0';
            }
        }
    };

    auto carveWallBetween = [&](int cx, int cy, int nx, int ny)
    {
        const int x1 = border + cx * step;
        const int y1 = border + cy * step;
        const int x2 = border + nx * step;
        const int y2 = border + ny * step;

        // 이웃 셀 방향에 따라 벽을 뚫음
        if (x2 > x1) // east
        {
            for (int y = y1; y < y1 + corridor && y < height - border; ++y)
            {
                for (int x = x1 + corridor; x < x1 + corridor + wall && x < width - border; ++x)
                {
                    mapData[y][x] = '0';
                }
            }
        }
        else if (x2 < x1) // west
        {
            for (int y = y1; y < y1 + corridor && y < height - border; ++y)
            {
                for (int x = x1 - wall; x < x1 && x >= border; ++x)
                {
                    mapData[y][x] = '0';
                }
            }
        }
        else if (y2 > y1) // south
        {
            for (int y = y1 + corridor; y < y1 + corridor + wall && y < height - border; ++y)
            {
                for (int x = x1; x < x1 + corridor && x < width - border; ++x)
                {
                    mapData[y][x] = '0';
                }
            }
        }
        else if (y2 < y1) // north
        {
            for (int y = y1 - wall; y < y1 && y >= border; ++y)
            {
                for (int x = x1; x < x1 + corridor && x < width - border; ++x)
                {
                    mapData[y][x] = '0';
                }
            }
        }
    };

    std::vector<std::vector<bool>> visited(cellsY, std::vector<bool>(cellsX, false));

    std::random_device rd;
    std::mt19937 rng(rd());

    std::stack<std::pair<int, int>> st;
    st.push({ 0, 0 });
    visited[0][0] = true;
    carveCell(0, 0);

    while (!st.empty())
    {
        int cx = st.top().first;
        int cy = st.top().second;


        std::vector<std::pair<int, int>> neighbors;
        if (cx > 0 && !visited[cy][cx - 1]) neighbors.push_back({ cx - 1, cy });
        if (cx + 1 < cellsX && !visited[cy][cx + 1]) neighbors.push_back({ cx + 1, cy });
        if (cy > 0 && !visited[cy - 1][cx]) neighbors.push_back({ cx, cy - 1 });
        if (cy + 1 < cellsY && !visited[cy + 1][cx]) neighbors.push_back({ cx, cy + 1 });

        if (neighbors.empty())
        {
            st.pop();
            continue;
        }

        std::shuffle(neighbors.begin(), neighbors.end(), rng);
        int nx = neighbors.back().first;
        int ny = neighbors.back().second;
        //auto [nx, ny] = neighbors.back();
        neighbors.pop_back();

        carveWallBetween(cx, cy, nx, ny);
        carveCell(nx, ny);
        visited[ny][nx] = true;
        st.push({ nx, ny });
    }

    // 파일 쓰기
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            mapFile << mapData[y][x];
        }
        mapFile << "\n";
    }

    mapFile.close();
    std::cout << "Maze generated: " << filename << std::endl;
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



