#include "pixiretro/pxr_gfx.h"
#include "Animation.h"
#include "AnimationFactory.h"

Animation::Animation() :
  _def{nullptr},
  _frameNo{0},
  _clock{0.f}
{}

Animation::Animation(std::shared_ptr<const Definition> def) :
  _def{def},
  _frameNo{0},
  _clock{0.f}
{
  assert(_def != nullptr);
}

void Animation::onUpdate(double now, float dt)
{
  assert(_def != nullptr);

  if(_def->_animationMode == Mode::STATIC)
    return;

  _clock += dt;
  if(_clock < _def->_period)
    return;

  switch(_def->_animationMode){
  case Mode::FORWARD:
    ++_frameNo;
    if(_frameNo >= _def->_frames.size())
      _frameNo = 0;
    break;
  case Mode::BACKWARD:
    --_frameNo;
    if(_frameNo < 0)
      _frameNo = _def->_frames.size() - 1;
    break;
  case Mode::RANDOM:
    _frameNo = pxr::rand::uniformUnsignedInt(0, _def->_frames.size() - 1);
    break;
  }
    
  _clock = 0.f;
}

void Animation::onDraw(Vector2i position, int screenid)
{
  assert(_def != nullptr); 
  gfx::drawSprite(position, _def->_spritesheetKey, _def->_frames[_frameNo], screenid);
}

void Animation::reset()
{
  assert(_def != nullptr);
  _frameNo = 0;
  _clock = 0.f;
}
