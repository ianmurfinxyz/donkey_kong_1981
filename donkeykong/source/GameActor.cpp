#include "GameActor.h"
#include "GamePlatform.h"


void GameActor::onInteraction(const std::vector<GameProp>& interactions)
{
  for(auto& prop : interactions){
    switch(prop.getType()){
    case GameProp::GAME_PROP_PLATFORM:
      GamePlatform* platform = static_cast<GamePlatform*>(&prop);
      if(platform->getSupportHeight() > _position._y)
        _position._y = platform->getSupportHeight();
      _isFalling = false;
      break;
    case GameProp::GAME_PROP_CONVEYOR:
      _isFalling = false;
      break;
    case GameProp::GAME_PROP_LADDER:
      break;
    case GameProp::GAME_PROP_LIFT:
      _isFalling = false;
      break;
    case GameProp::GAME_PROP_RIVET:
      break;
    }
  }
}

