#ifndef _PIXIRETRO_GAME_GAMEPROP_H_
#define _PIXIRETRO_GAME_GAMEPROP_H_

#include <vector>
#include "pixiretro/pxr_vec.h"
#include "pixiretro/pxr_rect.h"
#include "pixiretor/pxr_mathutil.h"
#include "pixiretro/pxr_collision.h"
#include "pixiretro/pxr_sfx.h"

class GameProp
{
  friend class GamePropFactory;
  
public:

  //
  // The transition mode defines how the prop choose which is the next state where in mode:
  //
  //  - FORWARD states are chosen in the order in which they are defined.
  //  - RANDOM states are chosen at random.
  //
  // note that each state has its own duration and the next state is chosen once the current
  // state has expired.
  //
  enum class StateTransitionMode { FORWARD, RANDOM };



  //
  // TODO
  //
  // when loading the state definitions in the prop factory need to assert the following 
  // preconditions:
  //
  //  Definition::_state.size() > 0
  //  StateDefinition::_displacementPoints.size() > 0
  //  StateDefinition::_velocityPoints.size() > 0
  //
  //  atleast 1 of all these must be provided in the prop definition or else the definition is
  //  invalid.
  //
  //


  struct StateDefinition
  {
    std::vector<std::pair<pxr::Vector2f, float>> _displacementPoints; 
    std::vector<std::pair<pxr::Vector2f, float>> _velocityPoints; 
    std::vector<std::tuple<std::string, pxr::sfx::ResourceKey_t, bool>> _sounds;
    pxr::fRect _interactionBox;
    std::string _animationName; 
    float _supportHeight;
    float _ladderHeight;
    pxr::Vector2i _conveyorVelocity;
    int _killerDamage;
    bool _isSupport;
    bool _isLadder;
    bool _isConveyor;
    bool _isKiller;
  };

  struct Definition
  {
    std::string _name;
    StateTransitionMode _stateTransitionMode;
    std::vector<GamePropStateDefinition> _states;
    int _drawLayer;
  };

  //
  // Default constructor needed for some STL containers.
  //
  GameProp();

  void onUpdate(double now, float dt);
  void onDraw(int screenid);

  bool isSupport() const;
  bool isLadder() const;
  bool isConveyor() const;
  bool isKiller() const;

  float getSupportHeight() const;
  float getLadderHeight() const;
  pxr::Vector2i getConveyorVelocity() const;
  int getKillerDamage() const;

  pxr::AABB getInteractionBox() const;

  int getDrawLayer() const;

private:

  GameProp(pxr::Vector2i position, const Definition* def);


  void transitionToState(int state);

  void lerpDisplacement(float dt);
  void lerpVelocity(float dt);

private:

  //
  // The definition encapsulates all type specific data and is shared by all instances 
  // of a specifix prop type.
  //
  const Definition* _def;

  //
  // Index into the Definition::_states vector which keeps track of the currently active
  // prop state.
  //
  int _currentState;

  //
  // All the animations used by the prop; one for each different prop state. This vector
  // is indexed by the _currentState index to access the animation for the corresponding
  // state.
  //
  std::vector<Animation> _animations;

  //
  // The position is the world space coordinate of the props local origin. This value
  // is fixed at construction. Props move by adding displacements given w.r.t the local
  // origin to this position.
  //
  pxr::Vector2f _position;

  //
  // The current displacement of the prop w.r.t its local origin.
  //
  pxr::Vector2f _displacement;

  //
  // The current velocity of the prop (w.r.t both world space or local space since local space
  // is a subspace of world space, is axis aligned and of the same scale).
  //
  pxr::Vector2i _velocity;

  //
  // Props are registered as not moving if there is only a single displacement point in the 
  // current state. This is a simple performance optimazation to avoid wasted work.
  //
  bool _isMoving;

  //
  // Indexes into the StateDefinition::_diplacementPoints vector which track the two 
  // displacements currently being lerped between. Only used if _isMoving == true.
  //
  int _toDisplacementPoint;
  int _fromDisplacementPoint;

  //
  // The clock used to track progress of linear interpolation of displacement points.
  //
  float _displacementLerpClock;

  //
  // Props are registered as not accelerating if there is only a single velocity point in the
  // current state or if _isMoving == false. This is a simple performance optimization to avoid 
  // wasted work.
  //
  bool _isAccelerating;

  //
  // Indexes into the StateDefinition::_velocityPoints vector which track the two 
  // velocities currently being lerped between. Only used if _isAccelerating == true;
  //
  int _toVelocityPoint;
  int _fromVelocityPoint;

  //
  // The clock used to track progress of linear interpolation of velocity points.
  //
  float _velocityLerpClock;
};

#endif
