#pragma once
#include <vector>
#include <functional>
#include "Math/Color.h"
#include "Actor/Actor.h"
#include "Util/Util.h"

using namespace Wanted;


class Player;

class LevelUpOverlay : public Actor
{
    RTTI_DECLARATIONS(LevelUpOverlay, Actor)

    struct LevelUpChoice
    {
        const char* text;
        std::function<void()> onSelected;
    };

public:
    explicit LevelUpOverlay(Player* _player);

    void Tick(float deltaTime) override;
    void Draw();

    void ShowLevelUpUI();
    void HideLevelUpUI();



    bool IsFinished() const { return finished; }

private:
    Player* player = nullptr;
    std::vector<LevelUpChoice> choices;
    int currentIndex = 0;
    bool finished = true;

    Color selectedColor = Color::Green;
    Color unselectedColor = Color::White;
};

