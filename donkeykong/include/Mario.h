#ifndef _PIXIRETRO_GAME_MARIO_H_
#define _PIXIRETRO_GAME_MARIO_H_

#include <array>
#include <string>
#include <memory>

#include "pixiretro/pxr_collision.h"
#include "pixiretro/pxr_sfx.h"
#include "ControlScheme.h"
#include "Animation.h"

class Mario
{
  friend class MarioFactory;

public:

  enum State
  {
    STATE_IDLE, 
    STATE_MOVING_LEFT,
    STATE_MOVING_RIGHT,
    STATE_CLIMBING_UP,
    STATE_CLIMBING_DOWN,
    STATE_JUMPING,
    STATE_FALLING,
    STATE_SPAWNING,
    STATE_DYING,
    STATE_COUNT
  };

  struct Definition
  {

    Definition(std::array<std::string, STATE_COUNT>                            animationNames,
               std::array<std::pair<pxr::sfx::ResourceKey_t, bool> STATE_COUNT sounds,
               float                                                           moveSpeed,
               float                                                           climbSpeed,
               float                                                           fallSpeed,
               float                                                           jumpSpeed,
               int                                                             spawnHealth);

    //
    // Animations to play during states (one for each state).
    //
    std::array<std::string, STATE_COUNT> _animationNames;

    //
    // Sounds to play on state entry (zero or one for each state). If a state has zero
    // sounds to play then the resource key == -1. The assiciated bool is a loop flag which
    // indicates if the sound should loop.
    //
    std::array<std::pair<pxr::sfx::ResourceKey_t, bool>, STATE_COUNT> _sounds;

    float _moveSpeed; 
    float _climbSpeed;
    float _fallSpeed;
    float _jumpSpeed;
    float _jumpDuration;
    int _spawnHealth;
  };

  void onPropCollisions(const std::vector<Prop>& props);
  void onBarrelCollisions(const std::vector<Barrel>& barrels);
  void onPickupCollisions(const std::vector<Pickup>& pickups);

  void onInput();
  void onUpdate(double now, float dt);
  void onDraw(int screenid);

  void changeState(State state);

  pxr::AABB getBarrelBox() const;
  pxr::AABB getPropBox() const;
  pxr::AABB getPickupBox() const;

  void respawn();

  bool isDead() const {return _isDead;}

private:

  //
  // Accessible only by the mario factory.
  //
  // Starts mario off in a dead state. Must call 'respawn()' to get mario setup.
  //
  Mario(pxr::Vector2f                        position, 
        std::shared_ptr<const ControlScheme> controlScheme, 
        std::shared_ptr<const Definition>    def);

  void startIdle();
  void endIdle();
  void startMoveLeft();
  void endMoveLeft();
  void startMoveRight();
  void endMoveRight();
  void startClimbUp();
  void endClimbUp();
  void startClimbDown();
  void endClimbDown();
  void startJump();
  void endJump();
  void startFall();
  void endFall();
  void startDying();
  void endDying();

private:

  std::shared_ptr<const Definition> _def;

  std::shared_ptr<const ControlScheme> _controlScheme;

  State _state;

  pxr::Vector2f _position;

  pxr::Vector2f _effectVelocity;
  pxr::Vector2f _controlVelocity;
  
  pxr::fRect _barrelBox;
  pxr::fRect _PropBox;
  pxr::fRect _pickupBox;

  int _health;

  float _jumpClock;

  Animation _animation;

  bool _isDead;
};

#endif
