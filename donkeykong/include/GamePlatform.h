#ifndef _DONKEYKONG_GAMEPLATFORM_H_
#define _DONKEYKONG_GAMEPLATFORM_H_

#include "GameProp.h"

class GamePlatform : public GameProp
{
public:
  GamePlatform(Vector2f position, float supportWidth, float supportHeight);

  inline float getSupportHeight();

private:
  float _supportWidth;
  float _supportHeight;
};

#endif
