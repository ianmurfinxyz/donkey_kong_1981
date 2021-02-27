#ifndef _PIXIRETRO_GAME_LEVEL_H_
#define _PIXIRETRO_GAME_LEVEL_H_

#include <memory>
#include <vector>
#include "GameProp.h"

class Level
{
public:

  enum State
  {
    STATE_ENTRANCE_CUTSCENE,
    STATE_PLAYING,
    STATE_EXIT_CUTSCENE
  };

  static constexpr const char* RESOURCE_PATH_LEVEL {"assets/levels/"};

  //
  // Load a level from a level file. If a level has already been loaded will overwrite the
  // existing level replacing it with the newly loaded level.
  //
  bool load(const std::string& file);

  void onUpdate(double now, float dt);

  void onDraw(int screenid);

  void reset();

private:

  void switchState(State state);

  void startEntranceCutscene();
  void endEntraceCutscene();
  void startPlaying();
  void endPlaying();
  void startExitCutscene();
  void endExitCutscene();

  void updateEntraceCutscene(double now, float dt);
  void updatePlaying(double now, float dt);
  void updateExitCutscene(double now, float dt);

private:

  State _state;

  std::vector<GameProp> _props;

  pxr::Vector2f _marioSpawn;
  std::unique_ptr<Mario> _mario;
};


#endif
