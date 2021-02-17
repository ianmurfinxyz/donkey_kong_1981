#include "GameProp.h"


GameProp::GameProp()
{
}

GameProp::GameProp(pxr:Vector2i position, const Definition* def) : 
  _def{def},
  _currentState{0},
  _animations{},
  _position{position},
  _velocity{0.f, 0.f},
  _isMoving{true},
  _isAccelerating{true}
{
  assert(_def != nullptr);



}

void GameProp::TransitionToState(int state)
{
  assert(0 <= state && state < _def->_states.size());

  if(_def->_states[0]._displacementPoints.size() == 1){
    _isMoving = false;
    _isAccelerating = false;
  }

  if(_isMoving && _def->_states[0]._velocityPoints.size() == 1)
    _isAccelerating = false;

  // set the initial displacement of the state
  lerpDisplacement(0);

  _currentState = state;
}

void GameProp::lerpDisplacement(float dt)
{
  if(!_isMoving)
    return;

  _displacementLerpClock += dt;
}

