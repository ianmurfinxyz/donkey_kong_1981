#ifndef _PIXIRETRO_GAME_ANIMATION_H_
#define _PIXIRETRO_GAME_ANIMATION_H_

class AnimationFactory;
struct AnimationDefinition;

//
// TODO
//
// i want the animation instance class to be very cheap so it can easily be recreated 
// every time we enter a new game prop state, rather than creating a vector of them for
// each state (trade CPU for memory).
//
// The issue is that rand generator. How expensive is it to create new rand generators?
//

class Animation
{
  friend class AnimationFactory;

public:

  //
  // The mode controls how the animation chooses which frame to show next. In mode:
  //
  //  - STATIC only a single frame is shown (the first frame).
  //  - FORWARD frames are shown one after the other in the order they are defined.
  //  - BACKWARD frames are shown in the reverse order in which they are defined.
  //  - RANDOM frame selection is random; any frame may shown after any other.
  //
  enum class Mode { STATIC, FORWARD, BACKWARD, RANDOM };

  Animation(const Animation& other) = delete;
  Animation& operator=(const Animation& other) = delete;

  //
  // Allows use as an element of std::vector.
  //
  Animation(Animation&& other) = default;
  Animation& operator=(Animation&& other) = default;

  void onUpdate(double now, float dt);
  void onDraw(Vector2i position, int screenid);
  void reset();

private:

  //
  // A definition defines an animation type. These definition are loaded from the
  // animations definitions file by the animation factory.
  //
  struct Definition
  {
    std::string _name;
    Animation::Mode _mode;
    gfx::ResourceKey_t _spritesheetKey;
    std::vector<gfx::SpriteID_t> _frames;
    float _frequency;
    float _period;
    bool _loop;
  };

  //
  // Accessible only by the factory.
  //
  Animation(const Definition* def);

private:

  //
  // Defines the animation type.
  //
  const Definition* _def;

  std::unique_ptr<iRand> _randFrame;
  int _frame;
  float _clock;
};

#endif
