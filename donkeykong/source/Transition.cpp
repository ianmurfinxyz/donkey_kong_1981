#include <cmath>
#include <cassert>
#include <algorithm>
#include "Transition.h"
#include "pixiretro/pxr_mathutil.h"

Transition::Transition() :
  _positionPoints{nullptr},
  _speedPoints{nullptr},
  _position{0.f, 0.f},
  _direction{0.f, 0.f},
  _speed{0.f},
  _lerpClock{0.f},
  _fromPosition{0},
  _toPosition{0},
  _fromSpeed{0},
  _toSpeed{0},
  _isMoving{false},
  _isAccelerating{false}
{}

Transition::Transition(std::shared_ptr<const std::vector<pxr::Vector2f>> positionPoints,
                       std::shared_ptr<const std::vector<SpeedPoint>> speedPoints)
  :
  _positionPoints{nullptr},
  _speedPoints{nullptr}
{
  reset(positionPoints, speedPoints);
}

void Transition::reset()
{
  if(_positionPoints == nullptr || _speedPoints == nullptr)
    return;

  _isMoving = true;
  _isAccelerating = true;

  if((*_positionPoints).size() == 1)
    _isMoving = false;

  if(!_isMoving || (*_speedPoints).size() == 1)
    _isAccelerating = false;

  _fromPosition = 0;
  _toPosition = _isMoving ? 1 : 0;
  _position = (*_positionPoints)[_fromPosition];

  _direction = ((*_positionPoints)[_toPosition] - (*_positionPoints)[_fromPosition]).normalized();

  _fromSpeed = 0;
  _toSpeed = _isAccelerating ? 1 : 0;
  _speed = (*_speedPoints)[_fromSpeed]._value;

  _lerpClock = 0.f;
}

void Transition::reset(std::shared_ptr<const std::vector<pxr::Vector2f>> positionPoints,
                       std::shared_ptr<const std::vector<SpeedPoint>> speedPoints)
{
  assert((*positionPoints).size() >= 1);
  assert((*speedPoints).size() >= 1);

  for(const auto& point : (*_speedPoints))
    assert(point._duration > 0.f);

  _positionPoints = positionPoints;
  _speedPoints = speedPoints;
  reset();
}

pxr::Vector2f Transition::onUpdate(float dt)
{
  if(!_isMoving)
    return _position;

  if(_isAccelerating){
    _lerpClock += dt;
    float phase = _lerpClock / (*_speedPoints)[_fromSpeed]._duration;

    _speed = pxr::lerp<float>((*_speedPoints)[_fromSpeed]._value, 
                              (*_speedPoints)[_toSpeed]._value,
                              std::clamp(phase, 0.f, 1.f));

    if(phase >= 1.f){
      ++_fromSpeed;
      if(_fromSpeed >= (*_speedPoints).size())
        _fromSpeed = 0;

      ++_toSpeed;
      if(_toSpeed >= (*_speedPoints).size())
        _toSpeed = 0;

      _lerpClock = 0.f;
    }
  }

  float distance = _speed * dt;
  pxr::Vector2f displacement = _direction * distance;
  pxr::Vector2f remainder = (*_positionPoints)[_toPosition] - _position;
  float difference = remainder.lengthSquared() - (distance * distance);

  if(difference > 0.f){
    _position += displacement;
    return _position;
  }

  else {
    _position += remainder; 

    ++_fromPosition;
    ++_toPosition;
    if(_toPosition >= (*_positionPoints).size()){
      _fromPosition = 0;
      _toPosition = 1;
    }

    _direction = ((*_positionPoints)[_toPosition] - (*_positionPoints)[_fromPosition]).normalized();

    difference *= -1;
    _position += _direction * std::sqrt(difference);
  }

  return _position;
}

pxr::Vector2f Transition::getPosition() const
{
  return _position;
}

float Transition::getSpeed() const
{
  return _speed;
}

