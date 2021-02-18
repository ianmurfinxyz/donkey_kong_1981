#include "GameProp.h"
#include "AnimationFactory.h"

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
  transitionToState(0);

  for(const auto& state : def->_state)
    _animations.push_back(AnimationFactory::makeAnimation());
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

  if(_def->_states[0]._displacementPoints.size() == 1){
    _isMoving = false;
    _isAccelerating = false;
  }

  if(_isMoving && _def->_states[0]._velocityPoints.size() == 1)
    _isAccelerating = false;

  _fromDisplacementPoint = 0;
  _toDisplacementPoint = _isMoving ? 1 : 0;
  _displacmentLerpClock = 0.f;
  _displacement = def->_displacementPoints[0].first;

  _fromVelocityPoint = 0;
  _toVelocityPoint = _isAccelerating ? 1 : 0;
  _velocityLerpClock = 0.f;
  _velocity = def->_velocityPoints[0].first;

  _currentState = state;
}

void GameProp::lerpDisplacement(float dt)
{
  if(!_isMoving)
    return;

  auto& fromPoint = _def->_displacementPoints[_fromDisplacementPoint];
  auto& toPoint = _def->_displacementPoints[_toDisplacementPoint];

  _displacementLerpClock += dt;

  float phase = _displacementLerpClock / fromPoint.second;

  _displacement._x = pxr::flerp(fromPoint.first._x, toPoint.first._x, std::clamp(phase, 0.f, 1.f));
  _displacement._y = pxr::flerp(fromPoint.first._y, toPoint.first._y, std::clamp(phase, 0.f, 1.f));

  if(phase > 1.f){
    ++_toDisplacementPoint;
    if(_toDisplacementPoint >= _def->_displacementPoints.size())
      _toDisplacementPoint = 0;

    ++_fromDisplacementPoint;
    if(_fromDisplacementPoint >= _def->_displacementPoints.size())
      _fromDisplacementPoint = 0;

    _displacementLerpClock = 0.f;
  }
}

void GameProp::lerpVelocity(float dt)
{
  if(!_isAccelerating)
    return;

  auto& fromPoint = _def->_velocityPoints[_fromVelocityPoint];
  auto& toPoint = _def->_velocityPoints[_toVelocityPoint];

  _velocityLerpClock += dt;

  float phase = _velocityLerpClock / fromPoint.second;

  _velocity._x = pxr::flerp(fromPoint.first._x, toPoint.first._x, std::clamp(phase, 0.f, 1.f));
  _velocity._y = pxr::flerp(fromPoint.first._y, toPoint.first._y, std::clamp(phase, 0.f, 1.f));

  if(phase > 1.f){
    ++_toVelocityPoint;
    if(_toVelocityPoint >= _def->_velocityPoints.size())
      _toVelocityPoint = 0;

    ++_fromVelocityPoint;
    if(_fromVelocityPoint >= _def->_velocityPoints.size())
      _fromVelocityPoint = 0;

    _velocityLerpClock = 0.f;
  }
}

