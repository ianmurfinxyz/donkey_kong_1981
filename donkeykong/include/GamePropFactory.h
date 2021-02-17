#ifndef _PIXIRETRO_GAME_GAMEPROPFACTORY_H_
#define _PIXIRETRO_GAME_GAMEPROPFACTORY_H_

#include <unordered_map>
#include "GameProp.h"

class GamePropFactory
{
public:

  //
  //
  //
  static constexpr const char* PROP_DEFINITIONS_FILE_NAME {"gameprops"};

  //
  //
  //
  static constexpr const char* PROP_DEFINITIONS_FILE_PATH {"assets/"};

  //
  //
  //
  bool loadGamePropDefinitions();

  GameProp makeGameProp(const std::string& propName);

private:
  std::unordered_map<std::string, GameProp::Definition> _propDefs;
};


#endif
