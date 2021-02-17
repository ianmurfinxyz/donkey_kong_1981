#include "pixiretro/pxr_gfx.h"
#include "Animation.h"
#include "AnimationFactory.h"

Animation::Animation() :
  _def{nullptr},
  _frame{0},
  _clock{0.f},
  _randFrame(0, 0)
{}

Animation::Animation(const AnimationDefinition* def) :
  _def{def},
  _frame{0},
  _clock{0.f},
  _randFrame(0, def->_frames.size() - 1)
{}

void Animation::onUpdate(double now, float dt)
{
  assert(def != nullptr);

  if(def->_animationMode == Mode::STATIC)
    return;

  _clock += dt;
  if(_clock < def->_period)
    return;

  switch(def->_animationMode){
  case Mode::FORWARD:
    ++_frame;
    if(_frame >= def->_frames.size())
      _frame = 0;
    break;
  case Mode::BACKWARD:
    --_frame;
    if(_frame < 0)
      _frame = def->_frame.size() - 1;
    break;
  case Mode::RANDOM:
    _frame = _randFrame();
    break;
  }
    
  _clock = 0.f;
}

void Animation::onDraw(Vector2i position, int screenid)
{
  assert(def != nullptr); 
  gfx::drawSprite(position, def->_spritesheetKey, def->_frames[_frame], screenid);
}

void Animation::reset()
{
  assert(def != nullptr);
  _frame = 0;
  _clock = 0.f;
}
