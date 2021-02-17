#include "GameActor.h"
#include "GamePlatform.h"


void GameActor::onInteraction(const std::vector<GameProp>& interactions)
{
  for(auto& prop : interactions){
    if(prop.isSupport()){
      // move up to supported height if > current height (always move up)
    }
    if(prop.isLadder()){
      // enable vertical movement
    }
    if(prop.isConveyor()){
      // apply conveyor velocity
    }
    if(prop.isKiller()){
      // apply damage from prop
    }
  }
}

