#include <cassert>
#include "pixiretro/pxr_rand.h"
#include "Prop.h"
#include "AnimationFactory.h"

Prop::Prop(pxr::Vector2f position, std::shared_ptr<const Definition> def) :
  _def{def},
  _currentState{0},
  _stateClock{0.f},
  _animation{},
  _position{position},
  _transition{}
{
  assert(_def != nullptr);
  assert(_def->_states.size() >= 1);
  _isChangingStates = !(_def->_states.size() == 1);
  transitionToState(0);
}

Prop::StateDefinition::StateDefinition(
  std::shared_ptr<std::vector<pxr::Vector2f>>           positionPoints, 
  std::shared_ptr<std::vector<Transition::SpeedPoint>>  speedPoints,
  std::vector<pxr::sfx::ResourceKey_t>                  sounds,
  pxr::fRect                                            interactionBox,
  std::string                                           animationName,
  float                                                 duration,
  float                                                 supportHeight,
  float                                                 ladderHeight,
  pxr::Vector2f                                         conveyorVelocity,
  int                                                   killerDamage,
  bool                                                  isSupport,
  bool                                                  isLadder,
  bool                                                  isConveyor,
  bool                                                  isKiller)
  :
  _positionPoints{std::move(positionPoints)},
  _speedPoints{std::move(speedPoints)},
  _sounds{sounds},
  _interactionBox{interactionBox},
  _animationName{animationName},
  _duration{duration},
  _supportHeight{supportHeight},
  _ladderHeight{ladderHeight},
  _conveyorVelocity{conveyorVelocity},
  _killerDamage{killerDamage},
  _isSupport{isSupport},
  _isLadder{isLadder},
  _isConveyor{isConveyor},
  _isKiller{isKiller}
{
  assert((*_positionPoints).size() >= 1);
  assert((*_speedPoints).size() >= 1);
  assert(_interactionBox._w >= 0 && _interactionBox._h >= 0);
  assert(_animationName.size() > 0);
}

Prop::Definition::Definition(
  std::string                  name,
  StateTransitionMode          stateTransitionMode,
  std::vector<StateDefinition> states,
  int                          drawLayer)
  :
  _name{name},
  _stateTransitionMode{stateTransitionMode},
  _states{std::move(states)},
  _drawLayer{drawLayer}
{
  assert(_name.size() > 0);
  assert(_states.size() >= 1);
}


void Prop::onUpdate(double now, float dt)
{
  _animation.onUpdate(dt);
  _transition.onUpdate(dt);

  if(!_isChangingStates)
    return;

  _stateClock += dt;
  if(_stateClock < _def->_states[_currentState]._duration)
    return;

  int newState {_currentState};
  switch(_def->_stateTransitionMode){
    case StateTransitionMode::FORWARD:
      ++newState;
      if(newState >= _def->_states.size())
        newState = 0;
      break;
    case StateTransitionMode::RANDOM:
      newState = pxr::rand::uniformSignedInt(0, _def->_states.size() - 1);
      break;
  }
  transitionToState(newState);
}

void Prop::reset()
{
  _currentState = 0;
  transitionToState(0);
}

void Prop::onDraw(int screenid)
{
  _animation.onDraw(_position + _transition.getPosition(), screenid);
}

bool Prop::isSupport() const
{
  return _def->_states[_currentState]._isSupport;
}

bool Prop::isLadder() const
{
  return _def->_states[_currentState]._isLadder;
}

bool Prop::isConveyor() const
{
  return _def->_states[_currentState]._isConveyor;
}

bool Prop::isKiller() const
{
  return _def->_states[_currentState]._isKiller;
}

float Prop::getSupportPosition() const
{
  return _position._y + _def->_states[_currentState]._supportHeight;
}

pxr::Vector2f Prop::getLadderRange() const
{
  return pxr::Vector2f {
    _position._y,
    _position._y + _def->_states[_currentState]._ladderHeight
  };
}

pxr::Vector2f Prop::getConveyorVelocity() const
{
  return _def->_states[_currentState]._conveyorVelocity;
}

int Prop::getKillerDamage() const
{
  return _def->_states[_currentState]._killerDamage;
}

pxr::AABB Prop::getInteractionBox() const
{
  auto& rect = _def->_states[_currentState]._interactionBox; 
  pxr::AABB aabb {};
  aabb._xmin = _position._x + rect._x;
  aabb._xmax = _position._x + rect._x + rect._w;
  aabb._ymin = _position._y + rect._y;
  aabb._ymax = _position._y + rect._y + rect._h;
  return aabb;
}

int Prop::getDrawLayer() const
{
  return _def->_drawLayer;
}

pxr::Vector2f Prop::getPosition() const
{
  return _position + _transition.getPosition();
}

pxr::gfx::ResourceKey_t Prop::getSpritesheetKey() const
{
  return _animation.getSpritesheetKey();
}

pxr::gfx::SpriteId_t Prop::getSpriteId() const
{
  return _animation.getSpriteId();
}

void Prop::transitionToState(int state)
{
  assert(0 <= state && state < _def->_states.size());
  const auto& stateDef = _def->_states[state];
  _stateClock = 0.f;
  _animation = AnimationFactory::makeAnimation(stateDef._animationName);
  _transition.reset(stateDef._positionPoints, stateDef._speedPoints);
  _currentState = state;

  //
  // Play all state entry sounds.
  //
  for(auto soundKey : _def->_states[state]._sounds)
    pxr::sfx::playSound(soundKey);
}

