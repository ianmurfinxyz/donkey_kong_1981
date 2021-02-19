#include "GameProp.h"
#include "AnimationFactory.h"

GameProp::GameProp(pxr:Vector2i position, const Definition* def) : 
  _def{def},
  _currentState{0},
  _animations{},
  _position{position},
  _transition{}
{
  assert(_def != nullptr);
  transitionToState(0);

  for(const auto& state : def->_states){
    _animations.push_back(AnimationFactory::makeAnimation());
  }

  auto& currentStateDef = def->_states[_currentState]
  _transition.reset(def->
}

void GameProp::onUpdate(double now, float dt)
{

}

void GameProp::onDraw(int screenid)
{

}

void GameProp::transitionToState(int state)
{
  assert(0 <= state && state < _def->_states.size());

  const auto& stateDef = _def->_states[state];

  _transition.reset(stateDef._positionPoints, stateDef._speedPoints);

  _currentState = state;
}


