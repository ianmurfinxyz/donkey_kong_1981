#ifndef _PIXIRETRO_GAME_GAMEPROP_H_
#define _PIXIRETRO_GAME_GAMEPROP_H_

#include <vector>
#include "pixiretro/pxr_vec.h"
#include "pixiretro/pxr_rect.h"
#include "pixiretor/pxr_mathutil.h"
#include "pixiretro/pxr_collision.h"
#include "pixiretro/pxr_sfx.h"
#include "Transition.h"

class GameProp
{
private:

  friend class GamePropFactory;
  
public:

  //
  // Call periodically within the update tick.
  //
  void onUpdate(double now, float dt);

  //
  // Draw the prop to a screen.
  //
  void onDraw(int screenid);

  //
  // Returns knowledge of what effects this prop has on actors.
  //
  bool isSupport() const;
  bool isLadder() const;
  bool isConveyor() const;
  bool isKiller() const;

  //
  // Returns the y-axis position w.r.t world space which this prop will support actors at,
  // i.e. actors standing on this support stand at this height.
  //
  float getSupportPosition() const;

  //
  // Returns the range of y-axis position values w.r.t world space within which this prop
  // permits vertical movement.
  //
  pxr::Vector2f getLadderRange() const;

  //
  // Returns the velocity imparted upon actors by this prop.
  //
  pxr::Vector2f getConveyorVelocity() const;

  //
  // Returns the damage done to actors by this prop.
  //
  int getKillerDamage() const;

  //
  // The collision bounds of this props interaction.
  //
  pxr::AABB getInteractionBox() const;

  //
  // Defines the order (via painters algorithm) that this prop should be drawn relative to
  // other props.
  //
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
    //
    // Constructor to establish the following invariants:
    //
    //     - position points must have size >= 1
    //     - speed points must have size >= 1
    //     - interaction box must have a positive area.
    //     - animation name != the empty string ""
    //
    StateDefinition(std::shared_ptr<std::vector<pxr::Vector2f>>           positionPoints, 
                    std::shared_ptr<std::vector<Transition::SpeedPoint>>  speedPoints,
                    std::vector<pxr::sfx::ResourceKey_t>                  sounds,
                    pxr::fRect                                            interactionBox,
                    std::string                                           animationName,
                    float                                                 duration,
                    float                                                 supportHeight,
                    float                                                 ladderHeight,
                    pxr::Vector2f                                         conveyorVelocity,
                    int                                                   killerDamage,
                    bool                                                  isSupport,
                    bool                                                  isLadder,
                    bool                                                  isConveyor,
                    bool                                                  isKiller);

    StateDefinition(StateDefinition&&) = default;
    StateDefinition& operator=(StateDefinition&&) = default;

    StateDefinition(const StateDefinition&) = delete;
    StateDefinition& operator=(const StateDefinition&) = delete;

    //
    // Transition points used to initialize this states transition.
    //
    // These shared pointers reference "original copies" of this point data; all transition
    // objects for every instance of props with this definition will ultimately copy these
    // shared pointers. Thus these will be the last references upon shutdown. This is reflected
    // in the fact these are the only shared pointers to this data that are writable.
    //
    std::shared_ptr<std::vector<pxr::Vector2f>> _positionPoints; 
    std::shared_ptr<std::vector<Transition::SpeedPoint>> _speedPoints;

    //
    // The set of sounds to begin playing upon state entry; sounds do not loop but rather play
    // once upon entry and stop.
    //
    SoundSet_t _sounds;

    //
    // A box specified w.r.t the props local coordinate space which defines the area a game
    // actor must intersect to interact with this prop.
    //
    pxr::fRect _interactionBox;

    //
    // Must be the name of a valid animation constructable via the animation factory. This
    // animation will play on loop during this state.
    //
    std::string _animationName; 

    //
    // How long this prop states persists before transitioning to the next.
    //
    float _duration;

    //
    // Supports provide actors a platform to stand on, the support height is where they stand.
    //
    float _supportHeight;

    //
    // Ladders permit actors to move vertically, ladder height is how far they can move.
    //
    float _ladderHeight;

    //
    // The velocity applied to actors upon interaction.
    //
    pxr::Vector2f _conveyorVelocity;

    //
    // Applied to actors upon interaction if _isKiller=true. Note that a + value will remove
    // health, whereas a - value will add health.
    //
    int _killerDamage;

    //
    // flags which indicate what effects this prop has on actors.
    //
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
    //    - name != the empty string ""
    //    - size of vector states >= 1
    //
    Definition(std::string                  name
               StateTransitionMode          stateTransitionMode,
               std::vector<StateDefinition> states,
               int                          drawLayer);

    //
    // The name of this prop type used to identify it.
    //
    std::string _name;

    //
    // Method of choosing the next state.
    //
    StateTransitionMode _stateTransitionMode;

    //
    // All the states of this prop. Must have size >= 1.
    //
    std::vector<StateDefinition> _states;

    //
    // Defines the order in which props should be drawn (painters algorithm). Lower values
    // are drawn first.
    //
    int _drawLayer;
  };

private:

  //
  // Accesable only by the game prop factory.
  //
  GameProp(pxr::Vector2f position, const Definition* def);

  void transitionToState(int state);

private:

  //
  // The definition which defines this props type. The lifetime of the referenced data
  // is guaranteed by the game prop factory from game init to shutdown.
  //
  std::shared_ptr<const Definition> _def;

  //
  // Index into the Definition::_states vector which keeps track of the currently active
  // prop state.
  //
  int _currentState;

  //
  // Times state changes.
  //
  float _stateClock;

  //
  // The animation being currently drawn to represent the prop.
  //
  Animation _animation;

  //
  // The position is the world space coordinate of the props local origin. This value
  // is fixed at construction. Props move by adding displacements given w.r.t the local
  // origin to this position.
  //
  pxr::Vector2f _position;

  //
  // The transition for the current state.
  //
  Transition _transition;

  //
  // Optimisation flag to disable state changes for props which have only a single state.
  //
  bool _isChangingStates;
};

#endif
