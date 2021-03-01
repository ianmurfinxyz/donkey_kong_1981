#ifndef _PIXIRETRO_GAME_CONTROL_SCHEME_H_
#define _PIXIRETRO_GAME_CONTROL_SCHEME_H_

#include "pixiretro/pxr_input.h"

struct ControlScheme
{
  pxr::input::KeyCode _runLeftKey;
  pxr::input::KeyCode _runRightKey;
  pxr::input::KeyCode _jumpKey;
  pxr::input::KeyCode _climbUpKey;
  pxr::input::KeyCode _climbDownKey;
};

#endif
