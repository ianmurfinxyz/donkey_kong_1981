#ifndef _DONKEYKONG_PLAYSTATE_H_
#define _DONKEYKONG_PLAYSTATE_H_

#include <memory>

#include "pixiretro/pxr_app.h"
#include "pixiretro/pxr_input.h"
#include "Level.h"
#include "ControlScheme.h"

class PlayState final : public pxr::AppState
{
public:
  static constexpr const char* name {"play"};

  PlayState(pxr::App* owner);
  ~PlayState() = default;

  bool onInit();
  void onUpdate(double now, float dt);
  void onDraw(double now, float dt, int screenid);
  void onReset();

  std::string getName() const {return name;}

private:

  static constexpr const char* RESOURCE_PATH_DKCONFIG {"assets/"};
  static constexpr const char* RESOURCE_NAME_DKCONFIG {"dkconfig"};

  static constexpr pxr::input::KeyCode nextLevelCheatKey {pxr::input::KEY_m};
  static constexpr pxr::input::KeyCode prevLevelCheatKey {pxr::input::KEY_n};

  void onCheatInput();
  bool nextLevel(bool loop);
  bool prevLevel(bool loop);
  void handleLevelWin();
  void handleLevelLoss();

  bool loadDKConfig();

private:
  
  //
  // An ordered set of levels to play through, starting with _levelNames[0] and incrementing.
  //
  std::vector<std::string> _levelNames;
  int _currentLevel;
  Level _level;

  std::shared_ptr<ControlScheme> _controlScheme;

  int _marioLives;
  int _score;
  bool _isCheating; // TODO load this from the dkconfig
};

#endif
