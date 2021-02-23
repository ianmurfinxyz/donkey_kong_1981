#include "DonkeyKong.h"
#include "AnimationFactory.h"
#include "GamePropFactory.h"
#include "PlayState.h"
#include "Defines.h"

bool DonkeyKong::onInit()
{
  if(!AnimationFactory::initialize())
    return false;

  if(!GamePropFactory::initialize())
    return false;

  _active = std::shared_ptr<pxr::AppState>(new PlayState(this));
  _active->onInit();
  _states.emplace(_active->getName(), _active);

  _activeScreenid = pxr::gfx::createScreen(worldSize);

  return true;
}

void DonkeyKong::onShutdown()
{
  GamePropFactory::shutdown();
  AnimationFactory::shutdown();
}
