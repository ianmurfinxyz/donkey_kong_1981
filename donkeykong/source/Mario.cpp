#include <cassert>
#include "Mario.h"
#include "AnimationFactory.h"
#include "Prop.h"

#include <iostream>

Mario::Mario(pxr::Vector2f                        spawnPosition,
             std::shared_ptr<const ControlScheme> controlScheme,
             std::shared_ptr<const Definition>    def)
  :
  _def{def},
  _controlScheme{controlScheme},
  _state{STATE_DEAD},
  _spawnPosition{spawnPosition},
  _position{0.f, 0.f},
  _direction{0.f, 0.f},
  _fallStartY{0.f},
  _fallEndY{0.f},
  _effectVelocity{0.f, 0.f},
  _controlVelocity{0.f, 0.f},
  _health{0},
  _jumpClock{0.f},
  _spawnClock{0.f},
  _dyingClock{0.f},
  _animation{}
{}

Mario::Definition::Definition(
  std::array<std::string, Mario::STATE_COUNT>                              animationNames,
  std::array<std::pair<pxr::sfx::ResourceKey_t, bool>, Mario::STATE_COUNT> sounds,
  pxr::Vector2i                                                            size,
  pxr::fRect                                                               propInteractionBox,
  float                                                                    runSpeed,
  float                                                                    climbSpeed,
  float                                                                    jumpImpulse,
  float                                                                    jumpDuration,
  float                                                                    gravity,
  float                                                                    maxFall,
  int                                                                      spawnHealth,
  float                                                                    spawnDuration,
  float                                                                    dyingDuration)
  :
  _animationNames{std::move(animationNames)},
  _sounds{sounds},
  _size{size},
  _propInteractionBox{propInteractionBox},
  _runSpeed{runSpeed},
  _climbSpeed{climbSpeed},
  _jumpImpulse{jumpImpulse},
  _jumpDuration{jumpDuration},
  _gravity{gravity},
  _maxFall{maxFall},
  _spawnHealth{spawnHealth},
  _spawnDuration{spawnDuration},
  _dyingDuration{dyingDuration}
{}

void Mario::onInput()
{
  if(_state == STATE_DEAD)
    return;

  switch(_state){
    case STATE_IDLE:

      if(pxr::input::isKeyDown(_controlScheme->_runLeftKey)){
        _direction._x = -1.f;
        changeState(STATE_RUNNING);
      }

      else if(pxr::input::isKeyDown(_controlScheme->_runRightKey)){
        _direction._x = 1.f;
        changeState(STATE_RUNNING);
      }

      else if(pxr::input::isKeyDown(_controlScheme->_jumpKey))
        changeState(STATE_JUMPING);

      break;

    case STATE_RUNNING:

      if(_direction._x < 0 && !pxr::input::isKeyDown(_controlScheme->_runLeftKey))
        changeState(STATE_IDLE);

      if(_direction._x > 0 && !pxr::input::isKeyDown(_controlScheme->_runRightKey))
        changeState(STATE_IDLE);

      if(pxr::input::isKeyPressed(_controlScheme->_jumpKey))
        changeState(STATE_JUMPING);

      break;

    case STATE_CLIMBING_UP:
    case STATE_CLIMBING_DOWN:

    case STATE_JUMPING:
    case STATE_FALLING:
    case STATE_SPAWNING:
    case STATE_DYING:
    default:
      break;
  }

}

void Mario::onUpdate(double now, float dt)
{
  if(_state == STATE_DEAD)
    return;

  std::cout << "mario state = " << _state << std::endl;

  if(_health <= 0)
    changeState(STATE_DYING);

  if(_state == STATE_DYING){
    _dyingClock += dt;
    if(_dyingClock > _def->_dyingDuration)
      changeState(STATE_DEAD);
  }

  if(_state == STATE_JUMPING || _state == STATE_FALLING){
    _controlVelocity._y += _def->_gravity * dt;  
  }

  if(_state == STATE_JUMPING){
    _jumpClock += dt;
    if(_jumpClock > _def->_jumpDuration)
      changeState(STATE_FALLING);
  }

  else if(_state == STATE_SPAWNING){
    _spawnClock += dt;
    if(_spawnClock > _def->_spawnDuration)
      changeState(STATE_IDLE); 
  }

  pxr::Vector2f velocity = _effectVelocity + _controlVelocity;
  _position += velocity * dt;

  _animation.onUpdate(dt);
}

void Mario::onDraw(int screenid)
{
  if(_state == STATE_DEAD)
    return;

  _animation.onDraw(_position, screenid);
}

void Mario::onPropInteractions(const std::vector<const Prop*>& props)
{
  _effectVelocity.zero();
  bool isSupported {false};
  float highestSupportPosition {0};
  for(const auto& prop : props){
    if(prop->isSupport()){
      isSupported = true;
      float supportPosition = prop->getSupportPosition();
      if(supportPosition > highestSupportPosition){
        highestSupportPosition = supportPosition;
        _position._y = supportPosition + (_def->_size._y / 2);
      }
    }
    if(prop->isConveyor()){
      _effectVelocity += prop->getConveyorVelocity();
    }
  }

  if(isSupported && _state == STATE_FALLING)
    changeState(STATE_IDLE);

  if(!isSupported && (_state != STATE_JUMPING || _state != STATE_FALLING))
    changeState(STATE_FALLING);

}

pxr::AABB Mario::getPropInteractionBox() const
{
  auto& rect = _def->_propInteractionBox; 
  pxr::AABB aabb {};
  aabb._xmin = _position._x + rect._x;
  aabb._xmax = _position._x + rect._x + rect._w;
  aabb._ymin = _position._y + rect._y;
  aabb._ymax = _position._y + rect._y + rect._h;
  return aabb;
}

void Mario::changeState(State state)
{
  assert(0 <= state && state < STATE_COUNT);
  
  switch(_state){
    case STATE_IDLE:
      endIdle();
      break;
    case STATE_RUNNING:
      endRunning();
      break;
    case STATE_CLIMBING_UP:
      endClimbUp();
      break;
    case STATE_CLIMBING_DOWN:
      endClimbDown();
      break;
    case STATE_JUMPING:
      endJump();
      break;
    case STATE_FALLING:
      endFall();
      break;
    case STATE_SPAWNING:
      endSpawning();
      break;
    case STATE_DYING:
      endDying();
      break;
    default:
      break;
  }

  auto& oldSound = _def->_sounds[_state];
  if(oldSound.first != -1 && oldSound.second){
    pxr::sfx::stopSound(oldSound.first);
  }

  _state = state;

  switch(_state){
    case STATE_IDLE:
      startIdle();
      break;
    case STATE_RUNNING:
      startRunning();
      break;
    case STATE_CLIMBING_UP:
      startClimbUp();
      break;
    case STATE_CLIMBING_DOWN:
      startClimbDown();
      break;
    case STATE_JUMPING:
      startJump();
      break;
    case STATE_FALLING:
      startFall();
      break;
    case STATE_SPAWNING:
      startSpawning();
      break;
    case STATE_DYING:
      startDying();
      break;
    default:
      break;
  }

  _animation = AnimationFactory::makeAnimation(_def->_animationNames[_state]);

  //
  // Expects the animation (and thus the spritesheet) to be of mario running left, thus
  // mirror if running right.
  //
  _animation.setMirrorX(_direction._x > 0.f);

  auto& sound = _def->_sounds[_state];
  if(sound.first != -1){
    pxr::sfx::playSound(sound.first, sound.second);
  }
}

void Mario::respawn()
{
  changeState(STATE_SPAWNING);
}

void Mario::startIdle()
{
  _controlVelocity.zero();
}

void Mario::endIdle()
{
}

void Mario::startRunning()
{
  _controlVelocity = _direction * _def->_runSpeed;
}

void Mario::endRunning()
{
}

void Mario::startClimbUp()
{
  _controlVelocity = pxr::Vector2f(0.f, 1.f) * _def->_climbSpeed;
}

void Mario::endClimbUp()
{
  _controlVelocity.zero();
}

void Mario::startClimbDown()
{
  _controlVelocity = pxr::Vector2f(0.f, -1.f) * _def->_climbSpeed;
}

void Mario::endClimbDown()
{
  _controlVelocity.zero();
}

void Mario::startJump()
{
  _controlVelocity._y += _def->_jumpImpulse;
}

void Mario::endJump()
{
}

void Mario::startFall()
{
  _fallStartY = _position._y;
}

void Mario::endFall()
{
  _fallEndY = _position._y;

  float fallDistance = _fallEndY - _fallStartY;
  if(fallDistance > _def->_maxFall)
    _health = 0;

  _controlVelocity.zero();
}

void Mario::startSpawning()
{
  _position = _spawnPosition;
  _health = _def->_spawnHealth;
  _effectVelocity.zero();
  _controlVelocity.zero();
  _spawnClock = 0.f;
}

void Mario::endSpawning()
{
}

void Mario::startDying()
{
  _dyingClock = 0.f;
}

void Mario::endDying()
{
}

