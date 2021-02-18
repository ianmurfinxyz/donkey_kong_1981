#ifndef _PIXIRETRO_GAME_ANIMATION_FACTORY_H_
#define _PIXIRETRO_GAME_ANIMATION_FACTORY_H_

#include "Animation.h"

//
//
//
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
  // Attempts to load the animations definitions file. Returns true on a successful load
  // else false. Load errors are logged to log file.
  //
  // Must be called during your app initialization i.e. App::onInit() but after loading
  // sprites.
  //
  static bool initializeFactory();

  //
  // Makes an instance of an animation type.
  //
  // If no such animation type with name 'animationName' exists the function asserts(0).
  //
  static Animation makeAnimation(const std::string& animationName);

private:
  static AnimationFactory* instance;

private:
  AnimationFactory() = default;
  ~AnimationFactory() = default;

  bool loadAnimationDefinitions();

private:
  std::unordered_map<std::string, Animation::Definition> _defs;
};

#endif
