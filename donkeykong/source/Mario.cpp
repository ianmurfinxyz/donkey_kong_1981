#include "Mario.h"

void Mario::onInput()
{
  switch(_state){
    case STATE_IDLE:

      if(pxr::input::isKeyDown(_controlScheme->_moveLeftKey))
        changeState(STATE_MOVING_LEFT);

      else if(pxr::input::isKeyDown(_controlScheme->_moveRightKey))
        changeState(STATE_MOVING_RIGHT);

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
    case STATE_DYING:
  }

}

void Mario::onUpdate(double now, float dt)
{
  pxr::Vector2f velocity = _effectVelocity + _controlVelocity;
  _position += velocity * dt;

  if(_state == STATE_JUMPING){
    _jumpClock += dt;
    if(_jumpClock > _def->_jumpDuration){
      endJump();
      startFall();
    }
  }
}

void Mario::changeState(State state)
{
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
  }

  auto& oldSound = _def->_sounds[_state];
  if(sound.first != -1 && sound.second){
    pxr::sfx::stopSound(sound.first);
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
  }

  _animation = AnimationFactory::makeAnimation(_def->_animationNames[_state]);

  auto& sound = _def->_sounds[_state];
  if(sound.first != -1){
    pxr::sfx::playSound(sound.first, sound.second);
  }
}

void Mario::startIdle()
{
  _controlVelocity = 0;
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
  _controlVelocity = 0;
}

void Mario::startMoveRight()
{
  _controlVelocity = pxr::Vector2f(1.f, 0.f) * _def->_moveSpeed;
}

void Mario::endMoveRight()
{
  _controlVelocity = 0;
}

void Mario::startClimbUp()
{
  _controlVelocity = pxr::Vector2f(0.f, 1.f) * _def->_climbSpeed;
}

void Mario::endClimbUp()
{
  _controlVelocity = 0;
}

void Mario::startClimbDown()
{
  _controlVelocity = pxr::Vector2f(0.f, -1.f) * _def->_climbSpeed;
}

void Mario::endClimbDown()
{
  _controlVelocity = 0;
}

void Mario::startJump()
{
  _controlVelocity._y = _def->_jumpSpeed;
}

void Mario::endJump()
{
  _controlVelocity = 0;
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

void Mario::startDying()
{
}

void Mario::endDying()
{
}

