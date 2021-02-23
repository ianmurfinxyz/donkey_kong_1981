#ifndef _PIXIRETRO_GAME_LEVEL_H_
#define _PIXIRETRO_GAME_LEVEL_H_

#include <vector>
#include "GameProp.h"

class GameLevel
{
public:

  static constexpr const char* RESOURCE_PATH_LEVEL {"assets/levels/"};

  //
  // Load a level from a level file. If a level has already been loaded will overwrite the
  // existing level replacing it with the newly loaded level.
  //
  bool load(const std::string& file);

  void onUpdate(double now, float dt);

  void onDraw(int screenid);

private:
  std::vector<GameProp> _props;
};


#endif
