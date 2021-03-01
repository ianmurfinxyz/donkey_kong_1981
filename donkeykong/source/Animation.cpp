#include <cassert>
#include "pixiretro/pxr_gfx.h"
#include "pixiretro/pxr_rand.h"
#include "Animation.h"
#include "AnimationFactory.h"

Animation::Animation() :
  _def{nullptr},
  _frameNo{0},
  _clock{0.f},
  _mirrorX{false},
  _mirrorY{false}
{}

Animation::Animation(std::shared_ptr<const Definition> def) :
  _def{def},
  _frameNo{0},
  _clock{0.f},
  _mirrorX{false},
  _mirrorY{false}
{
  assert(_def != nullptr);
  _mirrorX = _def->_baseMirrorX;
  _mirrorY = _def->_baseMirrorY;
}

Animation::Definition::Definition(
  std::string                       name,
  Mode                              mode,
  pxr::gfx::ResourceKey_t           spritesheetKey,
  std::vector<pxr::gfx::SpriteId_t> frames,
  float                             frequency,
  bool                              baseMirrorX,
  bool                              baseMirrorY)
  :
  _name{name},
  _mode{mode},
  _spritesheetKey{spritesheetKey},
  _frames{std::move(frames)},
  _frequency{frequency},
  _baseMirrorX{baseMirrorX},
  _baseMirrorY{baseMirrorY}
{
  assert(_name.size() > 0);
  assert(_frequency >= 0.f);
  _period = 1.f / _frequency;
}

void Animation::onUpdate(float dt)
{
  assert(_def != nullptr);

  if(_def->_mode == Mode::STATIC)
    return;

  _clock += dt;
  if(_clock < _def->_period)
    return;

  switch(_def->_mode){
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

void Animation::onDraw(pxr::Vector2i position, int screenid)
{
  assert(_def != nullptr); 
  pxr::gfx::drawSprite(position, _def->_spritesheetKey, _def->_frames[_frameNo], screenid, 
                       _mirrorX, _mirrorY);
}

void Animation::reset()
{
  assert(_def != nullptr);
  _frameNo = 0;
  _clock = 0.f;
}

void Animation::setMirrorX(bool mirror)
{
   _mirrorX = mirror ? !(_def->_baseMirrorX) : _def->_baseMirrorX;
}

void Animation::setMirrorY(bool mirror)
{
   _mirrorY = mirror ? !(_def->_baseMirrorY) : _def->_baseMirrorY;
}

