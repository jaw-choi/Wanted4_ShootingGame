#include "LevelUpOverlay.h"
#include "Core/Input.h"
#include "Render/Renderer.h"
#include "Actor/Player.h"
#include "Level/Level.h"
#include "Level/GameLevel.h"

LevelUpOverlay::LevelUpOverlay(Player* _player)
    : player(_player)
{
    // Level이 Player*를 넘겨줬다는 전제
    // 선택 시 스탯 적용은 player를 통해서만 수행
    //ShowLevelUpUI();
    finished = false;
}

void LevelUpOverlay::Tick(float deltaTime)
{
    super::Tick(deltaTime);

    ShowLevelUpUI();
    const int length = (int)choices.size();
    if (length <= 0) return;

    if (Input::Get().GetKeyDown(VK_UP))
        currentIndex = (currentIndex - 1 + length) % length;

    if (Input::Get().GetKeyDown(VK_DOWN))
        currentIndex = (currentIndex + 1) % length;

    if (Input::Get().GetKeyDown(VK_SPACE) || Input::Get().GetKeyDown(VK_RETURN))
    {
        choices[currentIndex].onSelected();
        HideLevelUpUI();
    }


    // 보통 레벨업은 강제 선택이라 ESC로 닫기 막는 걸 추천
}

void LevelUpOverlay::Draw()
{
    Renderer::Get().Submit("LEVEL UP! Choose one:", Vector2(0, 0), Color::Red);

    for (int i = 0; i < (int)choices.size(); ++i)
    {
        Color col = (i == currentIndex) ? selectedColor : unselectedColor;
        Renderer::Get().Submit(choices[i].text, Vector2(0, 2 + i), col, 10);
    }
}

void LevelUpOverlay::ShowLevelUpUI()
{
    if (finished)
        return;

    //dynamic_cast<GameLevel*>(GetOwner())->ShowLevelUpUI();

    choices.push_back({
        "Max HP +10",
        [this]()
        {
            player->AddExperience(1);
            //player->HealToFull(); // 원하면 레벨업 시 풀피 같은 처리
        }
        });

    choices.push_back({
        "Move Speed +0.2",
        [this]()
        {
            player->AddExperience(1);
        }
        });

    choices.push_back({
        "Damage +1",
        [this]()
        {
            player->AddExperience(1);
        }
        });
    finished = !finished;
}

void LevelUpOverlay::HideLevelUpUI()
{
    Destroy();
    static_cast<GameLevel*>(GetOwner())->HideLevelUpUI();
}
