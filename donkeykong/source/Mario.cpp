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
  _climbClock{0.f},
  _isNearLadder{false},
  _ladderRange{0.f, 0.f},
  _animation{}
{}

Mario::Definition::Definition(
  std::array<std::string, Mario::STATE_COUNT>                              animationNames,
  std::array<std::pair<pxr::sfx::ResourceKey_t, bool>, Mario::STATE_COUNT> sounds,
  pxr::Vector2i                                                            size,
  pxr::fRect                                                               propInteractionBox,
  float                                                                    runSpeed,
  float                                                                    climbSpeed,
  float                                                                    climbOnOffDuration,
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
  _climbOnOffDuration{climbOnOffDuration},
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

  if(_state == STATE_IDLE){
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
  }

  else if(_state == STATE_RUNNING){
    if(_direction._x < 0 && !pxr::input::isKeyDown(_controlScheme->_runLeftKey))
      changeState(STATE_IDLE);

    if(_direction._x > 0 && !pxr::input::isKeyDown(_controlScheme->_runRightKey))
      changeState(STATE_IDLE);

    if(pxr::input::isKeyPressed(_controlScheme->_jumpKey))
      changeState(STATE_JUMPING);
  }

  if(_isNearLadder && (_state == STATE_IDLE || _state == STATE_RUNNING || _state == STATE_CLIMBING_IDLE)){
    if(pxr::input::isKeyDown(_controlScheme->_climbUpKey))
      changeState(STATE_CLIMBING_UP);

    else if(pxr::input::isKeyDown(_controlScheme->_climbDownKey)){
      if(_state == STATE_CLIMBING_IDLE)
        changeState(STATE_CLIMBING_DOWN);

      //
      // Prevents you climbing onto the ladder if you are at the bottom; can only climb
      // onto the top of the ladder.
      //
      else {
        float feetPosition = _position._y - (_def->_size._y / 2);
        if(feetPosition > _ladderRange._x + 4)
          changeState(STATE_CLIMBING_ON);
      }
    }
  }

  else if(_state == STATE_CLIMBING_UP && !pxr::input::isKeyDown(_controlScheme->_climbUpKey))
    changeState(STATE_CLIMBING_IDLE);

  else if(_state == STATE_CLIMBING_DOWN && !pxr::input::isKeyDown(_controlScheme->_climbDownKey))
    changeState(STATE_CLIMBING_IDLE);


}

void Mario::onUpdate(double now, float dt)
{
  if(_state == STATE_DEAD)
    return;

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

  pxr::Vector2f velocity = _effectVelocity + _controlVelocity;
  _position += velocity * dt;

  _animation.onUpdate(dt);

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

  else if(_state == STATE_CLIMBING_UP){
    if(_position._y > _ladderRange._y){
      changeState(STATE_CLIMBING_OFF);

      //
      // clamp to the top of the ladder in case of large frame spikes (large dt) so we
      // dont shoot off the top.
      //
      _position._y = _ladderRange._y;
    }
  }

  else if(_state == STATE_CLIMBING_DOWN){
    float feetPosition = _position._y - (_def->_size._y / 2);
    if(feetPosition < _ladderRange._x){
      changeState(STATE_FALLING);

      //
      // clamp to the bottom of the ladder in case of large frame spikes (large dt) so we
      // dont shoot off the bottom.
      //
      _position._y = _ladderRange._x + (_def->_size._y / 2);
    }
  }

  else if(_state == STATE_CLIMBING_OFF){
    _climbClock += dt;
    if(_climbClock > _def->_climbOnOffDuration)
      changeState(STATE_IDLE);
  }

  else if(_state == STATE_CLIMBING_ON){
    _climbClock += dt;
    if(_climbClock > _def->_climbOnOffDuration)
      changeState(STATE_CLIMBING_IDLE);
  }
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
  _ladderRange._x = -1.f;
  _ladderRange._y = -1.f;
  _isNearLadder = false;

  bool isSupported {false};
  float highestSupportPosition {0};

  for(const auto& prop : props){

    if(prop->isSupport()){
      isSupported = true;
      float supportPosition = prop->getSupportPosition();
      if(supportPosition > highestSupportPosition)
        highestSupportPosition = supportPosition;
    }

    if(prop->isConveyor()){
      _effectVelocity += prop->getConveyorVelocity();
    }
    
    if(prop->isLadder()){
      _isNearLadder = true;
      pxr::Vector2f range = prop->getLadderRange();

      //
      // x is the lower range value, y is the upper range value (a position range
      // within the y-axis of world space).
      //
      // We expand the range to account for multiple ladder interactions.
      //
      if(range._x < _ladderRange._x || _ladderRange._x < 0) _ladderRange._x = range._x;
      if(range._y > _ladderRange._y || _ladderRange._y < 0) _ladderRange._y = range._y;
    }
  }

  //
  // Handles landing on the floor.
  //
  if(isSupported && _state == STATE_FALLING){
    _position._y = highestSupportPosition + (_def->_size._y / 2);
    changeState(STATE_IDLE);
  }

  //
  // Handles falling if unsupported.
  //
  if(!isSupported && (_state == STATE_RUNNING || _state == STATE_IDLE))
    changeState(STATE_FALLING);

  //
  // Handles the scenario where the ladder moved.
  //
  if((_state == STATE_CLIMBING_IDLE  ||
      _state == STATE_CLIMBING_UP    ||
      _state == STATE_CLIMBING_DOWN) &&
     !_isNearLadder)
  {
    changeState(STATE_FALLING); 
  }

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
    case STATE_CLIMBING_IDLE:
      endClimbIdle();
      break;
    case STATE_CLIMBING_UP:
      endClimbUp();
      break;
    case STATE_CLIMBING_DOWN:
      endClimbDown();
      break;
    case STATE_CLIMBING_OFF:
      endClimbOff();
      break;
    case STATE_CLIMBING_ON:
      endClimbOn();
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
      beginIdle();
      break;
    case STATE_RUNNING:
      beginRunning();
      break;
    case STATE_CLIMBING_IDLE:
      beginClimbIdle();
      break;
    case STATE_CLIMBING_UP:
      beginClimbUp();
      break;
    case STATE_CLIMBING_DOWN:
      beginClimbDown();
      break;
    case STATE_CLIMBING_OFF:
      beginClimbOff();
      break;
    case STATE_CLIMBING_ON:
      beginClimbOn();
      break;
    case STATE_JUMPING:
      beginJump();
      break;
    case STATE_FALLING:
      beginFall();
      break;
    case STATE_SPAWNING:
      beginSpawning();
      break;
    case STATE_DYING:
      beginDying();
      break;
    default:
      break;
  }

  _animation = AnimationFactory::makeAnimation(_def->_animationNames[_state]);
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

void Mario::beginIdle()
{
  _controlVelocity.zero();
}

void Mario::endIdle()
{
}

void Mario::beginRunning()
{
  _controlVelocity = _direction * _def->_runSpeed;
}

void Mario::endRunning()
{
}

void Mario::beginClimbIdle()
{
  _controlVelocity.zero();
}

void Mario::endClimbIdle()
{
}

void Mario::beginClimbUp()
{
  _controlVelocity._x = 0.f;
  _controlVelocity._y = _def->_climbSpeed;
}

void Mario::endClimbUp()
{
  _controlVelocity._y = 0.f;
}

void Mario::beginClimbDown()
{
  _controlVelocity._x = 0.f;
  _controlVelocity._y = -(_def->_climbSpeed);
}

void Mario::endClimbDown()
{
  _controlVelocity._y = 0.f;
}

void Mario::beginClimbOff()
{
  _climbClock = 0.f;
}

void Mario::endClimbOff()
{
  _position._y += _def->_size._y / 2;
}

void Mario::beginClimbOn()
{
  _controlVelocity._x = 0.f;
  _climbClock = 0.f;
  _position._y -= _def->_size._y / 2;
}

void Mario::endClimbOn()
{
}

void Mario::beginJump()
{
  _controlVelocity._y += _def->_jumpImpulse;
}

void Mario::endJump()
{
}

void Mario::beginFall()
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

void Mario::beginSpawning()
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

void Mario::beginDying()
{
  _dyingClock = 0.f;
}

void Mario::endDying()
{
}

