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
private:

  friend class GamePropFactory;
  
public:

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
  // A state definition encapsulates all type specific data of a game prop unique to a prop
  // state. Instances of these definitions are constructed and managed by the GamePropFactory.
  //
  struct StateDefinition
  {
    using Duration_t = float;
    using fLerpSet_t = std::vector<std::pair<pxr::Vector2f, Duration_t>>;
    using SoundSet_t = std::vector<std::string, pxr::sfx::ResourceKey_t>;

    //
    // Constructor to establish the following invariants:
    //
    //     - displacement set must have size >= 1
    //     - velocity set must have size >= 1
    //     - animation name != the empty string ""
    //
    StateDefinition(fLerpSet_t    displacementPoints, 
                    fLerpSet_t    velocityPoints,
                    std::string   animationName,
                    SoundSet_t    sound,
                    pxr::fRect    interactionBox,
                    float         supportHeight,
                    float         ladderHeight,
                    pxr::Vector2i conveyorVelocity,
                    int           killerDamage,
                    bool          isSupport,
                    bool          isLadder,
                    bool          isConveyor,
                    bool          isKiller);

    StateDefinition(StateDefinition&&) = default;
    StateDefinition& operator=(StateDefinition&&) = default;

    StateDefinition(const StateDefinition&) = delete;
    StateDefinition& operator=(const StateDefinition&) = delete;

    //
    // Sets of points to interpolate between. First in the pair is the point value (a 
    // displacement or velocity), second is the duration of the interpolation, i.e. time
    // it takes to interpolate between this point and the next. For the last point in the
    // set, the duration is the time to interpolate between said last point and the first
    // point in the set.
    //
    // All durations are in unit seconds.
    //
    // All state definitions required a set size >= 1. For sets of 1 point, interpolation
    // is disabled and the corresponding value is static.
    //
    // Interpolations loop for the entire duration of the state.
    //
    fLerpSet_t _displacementPoints; 
    fLerpSet_t _velocityPoints; 

    //
    // The set of sounds to begin playing upon state entry.
    //
    SoundSet_t _sounds;

    //
    // A box specified w.r.t the props local coordinate space which defines the area a game
    // actor must intersect to interact with this prop.
    //
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

  //
  // The definition encapsulates all type specific data and is shared by all instances 
  // of a specifix prop type.
  //
  struct Definition
  {

    //
    // Constructor to establish the following invariants.
    //
    //    - std::string != the empty string ""
    //    - size of vector state >= 1
    //
    Definition(std::string                  name
               StateTransitionMode          stateTransitionMode,
               std::vector<StateDefinition> states,
               int                          drawLayer);


    std::string _name;
    StateTransitionMode _stateTransitionMode;
    std::vector<StateDefinition> _states;
    int _drawLayer;
  };

private:

  GameProp(pxr::Vector2i position, const Definition* def);

  void transitionToState(int state);
  void lerpDisplacement(float dt);
  void lerpVelocity(float dt);

private:

  //
  // The definition which defines this props type. The lifetime of the referenced data
  // is guaranteed by the game prop factory from game init to shutdown.
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
  int _fromDisplacementPoint;
  int _toDisplacementPoint;

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
  int _fromVelocityPoint;
  int _toVelocityPoint;

  //
  // The clock used to track progress of linear interpolation of velocity points.
  //
  float _velocityLerpClock;
};

#endif
