#ifndef _PIXIRETRO_GAME_LEVEL_H_
#define _PIXIRETRO_GAME_LEVEL_H_

#include <memory>
#include <vector>
#include "ControlScheme.h"
#include "Prop.h"
#include "Mario.h"

class PlayState;

class Level
{
public:

  static constexpr const pxr::input::KeyCode debugDrawToggleKey {pxr::input::KEY_z};

  enum State
  {
    STATE_UNLOADED = -2,
    STATE_UNINITIALIZED = -1,
    STATE_ENTRANCE_CUTSCENE = 0,
    STATE_PLAYING,
    STATE_EXIT_CUTSCENE,
    STATE_COUNT
  };

  static constexpr const char* RESOURCE_PATH_LEVEL {"assets/levels/"};

  Level();
  ~Level() = default;

  Level(const Level&) = delete;
  Level& operator=(const Level&) = delete;

  Level(Level&&) = default;
  Level& operator=(Level&&) = default;

  //
  // Load a level from a level file. If a level has already been loaded will overwrite the
  // existing level replacing it with the newly loaded level.
  //
  bool load(const std::string& file);

  //
  // Clears the level into a state ready to load another level, will also
  // require reinitializing. Allows a level instance to be reused.
  //
  void unload();

  //
  // Call post load to setup the level.
  //
  void onInit(const PlayState* owner);

  void onUpdate(double now, float dt);

  void onDraw(int screenid);

  //
  // Resets the level to its 'fresh' post load and initialize state.
  //
  void reset();

private:

  void changeState(State state);

  void startEntranceCutscene();
  void endEntranceCutscene();
  void startPlaying();
  void endPlaying();
  void startExitCutscene();
  void endExitCutscene();

  void updateEntranceCutscene(double now, float dt);
  void updatePlaying(double now, float dt);
  void updateExitCutscene(double now, float dt);

  void debugDraw(int screenid);

private:

  State _state;

  const PlayState* _owner;

  std::shared_ptr<const ControlScheme> _controlScheme;

  std::vector<Prop> _props;

  pxr::Vector2f _marioSpawnPosition;
  std::unique_ptr<Mario> _mario;

  std::vector<const Prop*> _propInteractions;

  bool _isDebugDraw;
};


#endif
