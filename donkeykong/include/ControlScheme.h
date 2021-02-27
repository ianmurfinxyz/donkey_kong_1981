#ifndef _PIXIRETRO_GAME_CONTROL_SCHEME_H_
#define _PIXIRETRO_GAME_CONTROL_SCHEME_H_

#include "pixiretro/pxr_input.h"

struct ControlScheme
{
  pxr::input::KeyCode _moveLeftKey;
  pxr::input::KeyCode _moveRightKey;
  pxr::input::KeyCode _jumpKey;
};

#endif
