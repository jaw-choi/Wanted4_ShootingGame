#include "LevelUpOverlay.h"
#include "Core/Input.h"
#include "Render/Renderer.h"
#include "Actor/Player.h"
#include "Level/Level.h"
#include "Level/GameLevel.h"
#include "Util/Util.h"

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
    const int length = 3;
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
    Renderer::Get().Submit("LEVEL UP! Choose one:", Vector2(Engine::Get().GetWidth()/2, 0), Color::Red);

    int currentY = 2; // 시작 위치

    for (int i = 0; i < 3; ++i)
    {
        Color col = (i == currentIndex) ? selectedColor : unselectedColor;
        //Renderer::Get().SubmitFromFile(choices[i].text, Vector2(0, 2 + i), col, 10);
        // SubmitFromFile이 파일의 줄 수만큼 계산된 '다음 Y 좌표'를 반환함
        currentY = Renderer::Get().SubmitFromFile(choices[i].text, Vector2(Engine::Get().GetWidth() / 4, currentY), col, 10);

         currentY += 1;
    }
}

void LevelUpOverlay::ShowLevelUpUI()
{
    if (finished)
        return;
    //int random = Util::Random(0, 15);

    //dynamic_cast<GameLevel*>(GetOwner())->ShowLevelUpUI();

    choices.push_back({
        "../Assets/choice1.txt",
        [this]()
        {
            player->AddExperience(1);
            //player->HealToFull(); // 원하면 레벨업 시 풀피 같은 처리
        }
        });

    choices.push_back({
        "../Assets/choice2.txt",
        [this]()
        {
            player->AddExperience(1);
        }
        });

    choices.push_back({
        "../Assets/choice3.txt",
        [this]()
        {
            player->AddExperience(1);
        }
        });
    choices.push_back({
    "../Assets/choice4.txt",
    [this]()
    {
        player->AddExperience(1);
    }
        });
    //shuffle
    for (int i = (int)choices.size() - 1; i > 0; --i)
    {
        int j = Util::Random(0, i);
        std::swap(choices[i], choices[j]);
    }

    finished = !finished;
}

void LevelUpOverlay::HideLevelUpUI()
{
    Destroy();
    static_cast<GameLevel*>(GetOwner())->HideLevelUpUI();
}
