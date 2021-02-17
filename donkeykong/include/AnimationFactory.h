#ifndef _PIXIRETRO_GAME_ANIMATION_FACTORY_H_
#define _PIXIRETRO_GAME_ANIMATION_FACTORY_H_

#include "pixiretro/pxr_gfx.h"

class Animation;

class AnimationFactory
{
public:

  //
  // The name of the xml file which contains all the animation definitions.
  //
  static constexpr ANIMATION_DEFINITIONS_FILE_NAME {"animations"};

  //
  // The expected dir (w.r.t game root dir) in which the animations definitions
  // file is expected to be found.
  //
  static constexpr ANIMATION_DEFINITIONS_FILE_PATH {"assets/animations/"};

  //
  // A definition defines an animation type. These definition are loaded from the
  // animations definitions file.
  //
  struct AnimationDefinition
  {
    std::string _animationName;
    std::string _spritesheetName;
    gfx::ResourceKey_t _spritesheetKey;
    Animation::Mode _animationMode;
    std::vector<int> _frames;
    float _frequency;
    float _period;
    bool _loop;
  };

public:
  AnimationFactory() = default;
  ~AnimationFactory() = default;

  //
  // Attempts to load the animations definitions file. Returns true on a successful load
  // else false. Load errors are logged to log file.
  //
  bool loadAnimationDefinitions();

  //
  // Makes an instance of an animation type.
  //
  // If no such animation type with name 'animationName' exists the function asserts(0).
  //
  Animation makeAnimation(const std::string& animationName);

private:
  std::unorder_map<std::string, AnimationDefinition> _defs;
};


#endif
