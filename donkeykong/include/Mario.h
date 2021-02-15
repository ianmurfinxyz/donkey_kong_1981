#ifndef _DONKEYKONG_MARIO_H_
#define _DONKEYKONG_MARIO_H_

#include "GameActor.h"

class Mario final : public GameActor
{
public:

  void onUpdate(double now, float dt);
  void onIntegrate(float dt);
  void onDraw(int screenid);

private:

};

#endif
