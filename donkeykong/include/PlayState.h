#ifndef _DONKEYKONG_PLAYSTATE_H_
#define _DONKEYKONG_PLAYSTATE_H_

#include <memory>

#include "pixiretro/pxr_app.h"
#include "GameLevel.h"

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
  GameLevel _level;
};

#endif
