#include "pixiretro/pxr_gfx.h"
#include "PlayState.h"

PlayState::PlayState(pxr::App* owner) :
  pxr::AppState(owner),
  _level{}
{}

bool PlayState::onInit()
{
  _controlScheme = std::make_shared<ControlScheme>();
  _controlScheme->_moveLeftKey = pxr::input::KEY_LEFT;
  _controlScheme->_moveRightKey = pxr::input::KEY_RIGHT;
  _controlScheme->_jumpKey = pxr::input::KEY_SPACE;

  if(!_level.load("testlevel"))
    return false;

  _level.onInit(_controlScheme);

  return true;
}

void PlayState::onUpdate(double now, float dt)
{
  _level.onUpdate(now, dt);
}

void PlayState::onDraw(double now, float dt, int screenid)
{
  pxr::gfx::clearScreenShade(1, screenid);
  _level.onDraw(screenid);
}

void PlayState::onReset()
{
  _level.reset();
}
