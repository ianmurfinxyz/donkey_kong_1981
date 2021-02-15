#ifndef _DONKEYKONG_PLAYSTATE_H_
#define _DONKEYKONG_PLAYSTATE_H_

#include <memory>

#include "pixiretro/app.h"
#include "GameLevel.h"

class PlayState final : public pxr::AppState
{
public:
  static constexpr const char* name {"play"};

  bool onInit();
  void onUpdate(double now, float dt);
  void onDraw(double now, float dt);
  void onReset();

  std::string getName() const {return name;}

private:
  std::unique_ptr<GameLevel> _level;
};

#endif
