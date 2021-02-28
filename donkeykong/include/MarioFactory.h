#ifndef _PIXIRETRO_GAME_MARIO_FACTORY_H_
#define _PIXIRETRO_GAME_MARIO_FACTORY_H_

#include <memory>
#include "Mario.h"

class MarioFactory
{
public:

  static constexpr const char* MARIO_DEFINITION_FILE_PATH {"assets/"};
  static constexpr const char* MARIO_DEFINITION_FILE_NAME {"mario"};

  static bool initialize();

  static void shutdown();

  static Mario makeMario(pxr::Vector2f spawnPosition, std::shared_ptr<const ControlScheme> controlScheme);

private:

  static std::unique_ptr<MarioFactory> instance;

private:

  MarioFactory() = default;

  bool loadMarioDefinition();

private:
  std::shared_ptr<Mario::Definition> _marioDefinition;
};


#endif
