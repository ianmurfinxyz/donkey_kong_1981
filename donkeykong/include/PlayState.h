#ifndef _DONKEYKONG_PLAYSTATE_H_
#define _DONKEYKONG_PLAYSTATE_H_

#include <memory>

#include "pixiretro/pxr_app.h"
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

  void onLevelWin();
  void onLevelLoss();

  std::shared_ptr<const ControlScheme> getControlScheme() const {return _controlScheme;}

private:

  static constexpr const char* RESOURCE_PATH_DKCONFIG {"assets/"};
  static constexpr const char* RESOURCE_NAME_DKCONFIG {"dkconfig"};

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
};

#endif
