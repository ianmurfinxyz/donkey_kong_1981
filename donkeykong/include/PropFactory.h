#ifndef _PIXIRETRO_GAME_PROPFACTORY_H_
#define _PIXIRETRO_GAME_PROPFACTORY_H_

#include <unordered_map>
#include "Prop.h"

class PropFactory final
{
public:

  ~PropFactory() = default;

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
  static Prop makeProp(pxr::Vector2f position, const std::string& propName);

private:

  static std::unique_ptr<PropFactory> instance;

private:

  PropFactory() = default;

  bool loadPropDefinitions();

private:
  std::unordered_map<std::string, std::shared_ptr<Prop::Definition>> _defs;
};


#endif
