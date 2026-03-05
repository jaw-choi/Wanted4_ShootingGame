#include "Actor/Actor.h"
#include "Util/Timer.h"
#include "Stat/Stat.h"
#include "Interface/IstatHolder.h"
#include <functional>

using namespace Wanted;

class TeamB : public Actor, public IStatHolder
{

    RTTI_DECLARATIONS(TeamB, Actor)

public:
    TeamB();
    virtual ~TeamB();

private:
    virtual void Tick(float deltaTime) override;
    virtual void Draw() override;

    void SpawnUnit();
    void DeleteUnit();

};
