#include "DonkeyKong.h"
#include "AnimationFactory.h"
#include "GamePropFactory.h"
#include "PlayState.h"
#include "Defines.h"

bool DonkeyKong::onInit()
{
  AnimationFactory::initialize();
  GamePropFactory::initialize();

  _active = std::shared_ptr<pxr::AppState>(new PlayState(this));
  _active->onInit();
  _states.emplace(_active->getName(), _active);

  _activeScreenid = pxr::gfx::createScreen(worldSize);
}

void DonkeyKong::onShutdown()
{
  GamePropFactory::shutdown();
  AnimationFactory::shutdown();
}
