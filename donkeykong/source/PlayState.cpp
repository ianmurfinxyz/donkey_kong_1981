#include "PlayState.h"

PlayState::PlayState(pxr::App* owner) :
  pxr::AppState(owner),
  _level{}
{}

bool PlayState::onInit()
{
  _level.load("testlevel"); 
}

void PlayState::onUpdate(double now, float dt)
{
  _level.onUpdate(now, dt);
}

void PlayState::onDraw(double now, float dt, int screenid)
{
  _level.onDraw(screenid);
}

void PlayState::onReset()
{
  _level.load("testlevel"); 
}
