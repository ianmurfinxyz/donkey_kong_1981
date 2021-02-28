#include <cassert>
#include "Mario.h"
#include "AnimationFactory.h"

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
  _effectVelocity{0.f, 0.f},
  _controlVelocity{0.f, 0.f},
  _health{0},
  _jumpClock{0.f},
  _animation{}
{}

Mario::Definition::Definition(
  std::array<std::string, Mario::STATE_COUNT>                              animationNames,
  std::array<std::pair<pxr::sfx::ResourceKey_t, bool>, Mario::STATE_COUNT> sounds,
  float                                                                    moveSpeed,
  float                                                                    climbSpeed,
  float                                                                    fallSpeed,
  float                                                                    jumpSpeed,
  float                                                                    jumpDuration,
  int                                                                      spawnHealth)
  :
  _animationNames{std::move(animationNames)},
  _sounds{sounds},
  _moveSpeed{moveSpeed},
  _climbSpeed{climbSpeed},
  _fallSpeed{fallSpeed},
  _jumpSpeed{jumpSpeed},
  _jumpDuration{jumpDuration},
  _spawnHealth{spawnHealth}
{}

void Mario::onInput()
{
  if(_state == STATE_DEAD)
    return;

  std::cout << "mario_state = " << (int)_state << std::endl;

  switch(_state){
    case STATE_IDLE:

      if(pxr::input::isKeyDown(_controlScheme->_moveLeftKey))
        changeState(STATE_MOVING_LEFT);

      else if(pxr::input::isKeyDown(_controlScheme->_moveRightKey))
        changeState(STATE_MOVING_RIGHT);

      else if(pxr::input::isKeyDown(_controlScheme->_jumpKey))
        changeState(STATE_JUMPING);

      break;

    case STATE_MOVING_LEFT:

      if(!pxr::input::isKeyDown(_controlScheme->_moveLeftKey))
        changeState(STATE_IDLE);

      if(pxr::input::isKeyPressed(_controlScheme->_jumpKey))
        changeState(STATE_JUMPING);

      break;

    case STATE_MOVING_RIGHT:

      if(!pxr::input::isKeyDown(_controlScheme->_moveRightKey))
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

  pxr::Vector2f velocity = _effectVelocity + _controlVelocity;
  _position += velocity * dt;

  _animation.onUpdate(dt);

  if(_state == STATE_JUMPING){
    _jumpClock += dt;
    if(_jumpClock > _def->_jumpDuration){
      changeState(STATE_FALLING);
    }
  }

  else if(_state == STATE_SPAWNING){
    _spawnClock += dt;
    std::cout << "spawn clock = " << _spawnClock << "secs" << std::endl;
    if(_spawnClock > _def->_spawnDuration){
      changeState(STATE_IDLE); 
    }
  }
}

void Mario::onDraw(int screenid)
{
  if(_state == STATE_DEAD)
    return;

  _animation.onDraw(_position, screenid);
}

void Mario::changeState(State state)
{
  assert(0 <= state && state < STATE_COUNT);
  
  switch(_state){
    case STATE_IDLE:
      endIdle();
      break;
    case STATE_MOVING_LEFT:
      endMoveLeft();
      break;
    case STATE_MOVING_RIGHT:
      endMoveRight();
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
    case STATE_MOVING_LEFT:
      startMoveLeft();
      break;
    case STATE_MOVING_RIGHT:
      startMoveRight();
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
    case STATE_DYING:
      startDying();
      break;
    default:
      break;
  }

  _animation = AnimationFactory::makeAnimation(_def->_animationNames[_state]);

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

void Mario::startMoveLeft()
{
  _controlVelocity = pxr::Vector2f(-1.f, 0.f) * _def->_moveSpeed;
}

void Mario::endMoveLeft()
{
}

void Mario::startMoveRight()
{
  _controlVelocity = pxr::Vector2f(1.f, 0.f) * _def->_moveSpeed;
}

void Mario::endMoveRight()
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
  _controlVelocity._y = _def->_jumpSpeed;
}

void Mario::endJump()
{
  _controlVelocity.zero();
}

void Mario::startFall()
{
  _controlVelocity._y = -(_def->_fallSpeed);
  _jumpClock = 0.f;
}

void Mario::endFall()
{
  _controlVelocity._y = 0.f;
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
}

void Mario::endDying()
{
}

