#include <cmath>
#include "Transition.h"
#include "pixiretro/pxr_mathutil.h"

Transition::Transition(std::vector<pxr::Vector2f> positionPoints,
                       std::vector<SpeedPoints> speedPoints)
  :
  _positionPoints{std::move(positionPoints)},
  _speedPoints{std::move(speedPoints)}
{
  assert(_positionPoints.size() >= 1);
  assert(_speedPoints.size() >= 1);

  reset();
}

void Transition::reset()
{
  _isMoving = true;
  _isAccelerating = true;

  if(_positionPoints.size() == 1)
    _isMoving = false;

  if(!_isMoving || _speedPoints.size() == 1)
    _isAccelerating = false;

  _fromPosition = 0;
  _toPosition = _isMoving ? 1 : 0;
  _position = _positionPoints[_fromPosition];

  _direction = (_positionPoints[_toPosition] - _positionPoints[_fromPosition]).normalized();

  _fromSpeed = 0;
  _toSpeed = _isAccelerating ? 1 : 0;
  _speed = _speedPoints[_fromSpeed]._value;

  _lerpClock = 0.f;
}

pxr::Vector2f Transition::update(float dt)
{
  if(!_isMoving)
    return;

  if(_isAccelerating){
    _lerpClock += dt;
    float phase = _lerpClock / _speedPoints[_fromSpeed]._duration;

    _speed = pxr::flerp(_speedPoints[_fromSpeed]._value, 
                        _speedPoints[_toSpeed]._value,
                        std::clamp(phase, 0.f, 1.f));

    if(phase >= 1.f){
      ++_fromSpeed;
      if(_fromSpeed >= _speedPoints.size())
        _fromSpeed = 0;

      ++_toSpeed;
      if(_toSpeed >= _speedPoints.size())
        _toSpeed = 0;

      _lerpClock = 0.f;
    }
  }

  float distance = _speed * dt;
  Vector2f displacement = _direction * distance;
  Vector2f remainder = _speedPoints[_toSpeed]._value - _position;
  float difference = remainder.lengthSquared() - (distance * distance);

  if(difference > 0.f){
    _position += displacement;
    return;
  }

  else {
    _position += remainder; 

    ++_fromPosition;
    ++_toPosition;
    if(_toPosition >= _positionPoints.size()){
      _fromPosition = 0;
      _toPosition = 1;
    }

    _direction = (_positionPoints[_toPosition] - _positionPoints[_fromPosition]).normalized();

    difference *= -1;
    _position += std::sqrt(difference) * _direction;
  }
}

pxr::Vector2f Transition::getPosition() const
{
  return _position;
}

float Transition::getSpeed() const
{
  return _speed;
}

