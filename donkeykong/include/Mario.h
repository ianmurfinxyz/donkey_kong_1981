#ifndef _PIXIRETRO_GAME_MARIO_H_
#define _PIXIRETRO_GAME_MARIO_H_

#include <array>
#include <string>
#include <memory>

#include "pixiretro/pxr_collision.h"
#include "pixiretro/pxr_sfx.h"
#include "pixiretro/pxr_vec.h"
#include "pixiretro/pxr_rect.h"
#include "ControlScheme.h"
#include "Animation.h"

class Mario
{
  friend class MarioFactory;

public:

  enum State
  {
    STATE_DEAD = -1,
    STATE_IDLE = 0, 
    STATE_RUNNING,
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

    Definition(std::array<std::string, Mario::STATE_COUNT>                              animationNames,
               std::array<std::pair<pxr::sfx::ResourceKey_t, bool>, Mario::STATE_COUNT> sounds,
               float                                                                    runSpeed,
               float                                                                    climbSpeed,
               float                                                                    fallSpeed,
               float                                                                    jumpSpeed,
               float                                                                    jumpDuration,
               int                                                                      spawnHealth);

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

    //pxr::fRect _barrelBox;
    //pxr::fRect _PropBox;
    //pxr::fRect _pickupBox;

    float _runSpeed; 
    float _climbSpeed;
    float _fallSpeed;
    float _jumpSpeed;
    float _jumpDuration;
    int _spawnHealth;
    float _spawnDuration;
  };

  Mario(const Mario&) = default;
  Mario& operator=(const Mario&) = default;

  Mario(Mario&&) = default;
  Mario& operator=(Mario&&) = default;

  //
  // Resets mario back to its initial spawn state. Must call respawn initially after
  // contruction to put mario into a spawned/playing state (constructor creates a dead mario).
  //
  void respawn();

  void onInput();
  void onUpdate(double now, float dt);
  void onDraw(int screenid);

  //void onPropCollisions(const std::vector<Prop>& props);
  //void onBarrelCollisions(const std::vector<Barrel>& barrels);
  //void onPickupCollisions(const std::vector<Pickup>& pickups);

  //pxr::AABB getBarrelBox() const;
  //pxr::AABB getPropBox() const;
  //pxr::AABB getPickupBox() const;


  bool isDead() const {return _state == STATE_DEAD;}

  void setSpawnPosition(pxr::Vector2f spawnPosition) {_spawnPosition = spawnPosition;}
  pxr::Vector2f getSpawnPosition() const {return _spawnPosition;}

private:

  //
  // Accessible only by the mario factory.
  //
  // Starts mario off in a dead state. Must call 'respawn()' to get mario setup.
  //
  Mario(pxr::Vector2f                        spawnPosition, 
        std::shared_ptr<const ControlScheme> controlScheme, 
        std::shared_ptr<const Definition>    def);

  void changeState(State state);

  void startIdle();
  void endIdle();
  void startRunning();
  void endRunning();
  void startClimbUp();
  void endClimbUp();
  void startClimbDown();
  void endClimbDown();
  void startJump();
  void endJump();
  void startFall();
  void endFall();
  void startSpawning();
  void endSpawning();
  void startDying();
  void endDying();

private:

  std::shared_ptr<const Definition> _def;

  std::shared_ptr<const ControlScheme> _controlScheme;

  State _state;

  pxr::Vector2f _spawnPosition;
  pxr::Vector2f _position;

  pxr::Vector2f _direction;

  pxr::Vector2f _effectVelocity;
  pxr::Vector2f _controlVelocity;

  int _health;

  float _jumpClock;
  float _spawnClock;

  Animation _animation;
};

#endif
