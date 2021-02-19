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
    //
    // Constructor to establish the following invariants:
    //
    //     - position points must have size >= 1
    //     - speed points must have size >= 1
    //     - interaction box must have a positive area.
    //     - animation name != the empty string ""
    //
    StateDefinition(std::vector<pxr::Vector2f>           positionPoints, 
                    std::vector<Transition::SpeedPoint>  speedPoints,
                    std::vector<pxr::sfx::ResourceKey_t> sound,
                    pxr::fRect                           interactionBox,
                    std::string                          animationName,
                    float                                duration,
                    float                                supportHeight,
                    float                                ladderHeight,
                    pxr::Vector2i                        conveyorVelocity,
                    int                                  killerDamage,
                    bool                                 isSupport,
                    bool                                 isLadder,
                    bool                                 isConveyor,
                    bool                                 isKiller);

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
    pxr::Vector2i _conveyorVelocity;

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

    //
    // Defines the order in which props should be drawn (painters algorithm). Lower values
    // are drawn first.
    //
    int _drawLayer;
  };

private:

  GameProp(pxr::Vector2i position, const Definition* def);

  void transitionToState(int state);

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
  // The transition for the current state.
  //
  Transition _transition;
};

#endif
