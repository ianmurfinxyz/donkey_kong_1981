#ifndef _PIXIRETRO_GAME_ANIMATION_H_
#define _PIXIRETRO_GAME_ANIMATION_H_

class AnimationFactory;
struct AnimationDefinition;

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

  //
  // Default constructor is needed for some STL containers.
  //
  Animation();

  Animation(const Animation& other) = default;
  Animation& operator=(const Animation& other) = default;

  Animation(Animation&& other) = delete;
  Animation& operator=(Animation&& other) = delete;

  void onUpdate(double now, float dt);
  void onDraw(Vector2i position, int screenid);
  void reset();

private:
  Animation(const AnimationDefinition* def);

private:
  iRand _randFrame;
  const AnimationDefinition* _def;
  int _frame;
  float _clock;
};

#endif
