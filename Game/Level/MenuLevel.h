#pragma once

#include "Level/Level.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include <vector>
#include <fstream>
#include <string>
#include <cstddef>

using namespace Wanted;


// 메뉴 아이템 구조체.
struct MenuItem
{
    // 메뉴 선택됐을 때 실행될 함수의 타입.
    using OnSelected = void (*)();

    MenuItem(const char* text, OnSelected onSelected)
	: onSelected(onSelected)
    {
	// 텍스트 복사.
	size_t length = strlen(text) + 1;
	this->text = new char[length];
	strcpy_s(this->text, length, text);
    }
    
    ~MenuItem()
    {
	if (text)
	{
	    delete[] text;
	    text = nullptr;
	}
    }

    // 메뉴 텍스트.
    char* text = nullptr;

    // 메뉴 선택됐을 때 실행될 로직(함수-함수 포인터).
    OnSelected onSelected = nullptr;
};

class Position
{
public:
    int x = 0;
    int y = 0;

    Position(int x, int y) : x(x), y(y)
    {
    }
};

class Node
{
public:
    Node(int x, int y, Node* parentNode = nullptr) : position(x, y), parentNode(parentNode)
    {
    }

    Position operator-(const Node& other) const
    {
	return Position(position.x - other.position.y, position.y - other.position.y);
    }

    bool operator==(const Node& other) const
    {
	return position.x == other.position.x && position.y == other.position.y;
    }

public:
    // 이 노드의 위치.
    Position position;

    // 시작 위치에서 이 노드까지의 비용.
    float gCost;

    // 이 노드에서 목표지점까지의 예상 비용(휴리스틱).
    float hCost;

    // 최종 비용(gCost + hCost)
    float fCost;

    // 부모 노드 링크.
    Node* parentNode = nullptr;
};


class AStar
{
    // 방향 처리를 위한 구조체.
    struct Direction
    {
	// 위치.
	int x = 0;
	int y = 0;

	// 이동 비용.
	float cost = 0.0f;
    };

public:
    AStar();
    ~AStar();

    enum class State
    {
	Idle,
	Searching,
	Found,
	Failed
    };

    void Begin(Node* startNode, Node* goalNode, std::vector<std::vector<int>>* grid);
    State Step();
    State GetState() const { return state; }
    const std::vector<Node*>& GetPath() const { return path; }
    const std::vector<Vector2>& GetExplored() const { return exploredOrder; }
    void SetOrigin(const Vector2& origin) { renderOrigin = origin; }

    // 객체 지향 이론에서 -> 메시지(Message) - 공개 멤버 함수(인터페이스).
    // 경로 검색(탐색) 함수.
    // startNode: 시작 지점.
    // goalNode: 목표 지점.
    // grid: 탐색할 맵 (2차원 배열).
    std::vector<Node*> FindPath(
	Node* startNode, Node* goalNode,
	std::vector<std::vector<int>>& grid
    );

    // 맵(그리드) 출력 함수.
    void DisplayGridWithPath(
	std::vector<std::vector<int>>& grid,
	const std::vector<Node*>& path,
	size_t maxNodes
    );
    void DisplayGrid(std::vector<std::vector<int>>& grid);
private:
    // 객체 지향 이론에서 -> 메소드(Method) - 비공개 멤버 함수.

    // 탐색을 완료한 후에 최적 경로 반환하는 함수.
    // 부모링크를 따라가면서 역추적.
    std::vector<Node*> ConstructPath(Node* goalNode);

    // 휴리스틱(hCost) 계산 함수.
    float CalculateHeuristic(Node* currentNode, Node* goalNode);

    // 탐색하려는 위치(노드)가 그리그 범위 안에 있는지 확인.
    // 유효성 검증.
    bool IsInRange(int x, int y, const std::vector<std::vector<int>>& grid);

    // 이미 방문했는지 확인하는 함수.
    bool HasVisited(int x, int y, float gCost);

    // 탐색하려는 노드가 목표 노드인지 확인하는 함수.
    bool IsDestination(const Node* const node);

private:
    void Reset();
    Node* FindLowestFCostNode() const;

    // 열린 리스트.
    std::vector<Node*> openList;

    // 닫힌 리스트.
    std::vector<Node*> closedList;

    // 시작 노드.
    Node* startNode = nullptr;

    // 목표 노드.
    Node* goalNode = nullptr;

    // 현재 탐색 상태.
    State state = State::Idle;

    // 현재 작업중인 그리드.
    std::vector<std::vector<int>>* gridPtr = nullptr;

    // 최종 경로.
    std::vector<Node*> path;

    // 그리드 렌더링 시작 위치.
    Vector2 renderOrigin = Vector2::Zero;

    // 탐색 순서(표시용)
    std::vector<Vector2> exploredOrder;
};

class MenuLevel : public Level
{
    RTTI_DECLARATIONS(MenuLevel, Level)

public:
    MenuLevel();
    ~MenuLevel();

    // 이벤트 함수 오버라이드.
    virtual void Tick(float deltaTime) override;
    virtual void Draw() override;

private:
    void InitAStarIfNeeded();
    void BuildGridRenderString();

    // 현재 활성화된 메뉴 아이템 인덱스.
    int currentIndex = 0;

    // 선택된 아이템의 색상.
    Color selectedColor = Color::Green;

    // 선택되지 않은 아이템의 색상.
    Color unselectedColor = Color::White;

    // 메뉴 아이템 배열.
    std::vector<MenuItem*> items;

    // A* 시각화용
    AStar aStar;
    std::vector<std::vector<int>> gridWork;
    bool astarInitialized = false;
    std::vector<Node*> finalPath;
    std::vector<Vector2> exploredNodes;
    float pathReveal = 0.0f;
    size_t pathRevealCount = 0;
    float pathRevealSpeed = 120.0f; // 노드/초
    float exploreReveal = 0.0f;
    size_t exploreRevealCount = 0;
    float exploreRevealSpeed = 200.0f; // 노드/초
    std::string gridRenderString;
};
