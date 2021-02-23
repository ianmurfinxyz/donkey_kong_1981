#ifndef _PIXIRETRO_GAME_GAMEPROPFACTORY_H_
#define _PIXIRETRO_GAME_GAMEPROPFACTORY_H_

#include <unordered_map>
#include "GameProp.h"

class GamePropFactory final
{
public:

  ~GamePropFactory() = default;

  //
  //
  //
  static constexpr const char* PROP_DEFINITIONS_FILE_PATH {"assets/"};

  //
  //
  //
  static constexpr const char* PROP_DEFINITIONS_FILE_NAME {"gameprops"};

  //
  //
  // Must be called after initializing the animation factory.
  //
  static bool initialize();

  //
  //
  //
  static void shutdown();

  //
  //
  //
  static GameProp makeGameProp(pxr::Vector2f position, const std::string& propName);

private:

  static std::unique_ptr<GamePropFactory> instance;

private:

  GamePropFactory() = default;

  bool loadGamePropDefinitions();

private:
  std::unordered_map<std::string, std::shared_ptr<GameProp::Definition>> _defs;
};


#endif
