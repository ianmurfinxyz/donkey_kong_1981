#ifndef _PIXIRETRO_GAME_ANIMATION_FACTORY_H_
#define _PIXIRETRO_GAME_ANIMATION_FACTORY_H_

#include <memory>
#include "Animation.h"

//
// Singleton class to instantiate animations. Responsible for loading and maintaining
// all animation definitions.
//
class AnimationFactory final
{
public:

  ~AnimationFactory() = default;

  //
  // The expected dir (w.r.t game root dir) in which the animations definitions
  // file is expected to be found.
  //
  static constexpr const char* ANIMATION_DEFINITIONS_FILE_PATH {"assets/"};

  //
  // The name of the xml file which contains all the animation definitions.
  //
  static constexpr const char* ANIMATION_DEFINITIONS_FILE_NAME {"animations"};

  //
  // Attempts to load the animations definitions file. Returns true on a successful load
  // else false. Load errors are logged to log file.
  //
  // Must be called during your app initialization i.e. App::onInit() but after loading
  // sprites.
  //
  static bool initialize();

  //
  // Must call to free memory used by the factory singleton instance.
  //
  static void shutdown();

  //
  // Makes an instance of an animation type.
  //
  // If no such animation type with name 'animationName' exists the function asserts(0).
  //
  static Animation makeAnimation(const std::string& animationName);

private:
  static std::unique_ptr<AnimationFactory> instance;

private:
  AnimationFactory() = default;

  bool loadAnimationDefinitions();

private:
  std::unordered_map<std::string, std::shared_ptr<Animation::Definition>> _defs;
};

#endif
