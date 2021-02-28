#include "DonkeyKong.h"
#include "AnimationFactory.h"
#include "PropFactory.h"
#include "MarioFactory.h"
#include "PlayState.h"
#include "Defines.h"

bool DonkeyKong::onInit()
{
  if(!AnimationFactory::initialize())
    return false;

  if(!PropFactory::initialize())
    return false;

  if(!MarioFactory::initialize())
    return false;

  _active = std::shared_ptr<pxr::AppState>(new PlayState(this));

  if(!_active->onInit())
    return false;

  _states.emplace(_active->getName(), _active);

  _activeScreenid = pxr::gfx::createScreen(worldSize);

  return true;
}

void DonkeyKong::onShutdown()
{
  PropFactory::shutdown();
  AnimationFactory::shutdown();
  MarioFactory::shutdown();
}
