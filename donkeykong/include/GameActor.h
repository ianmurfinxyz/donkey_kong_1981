#ifndef _DONKEYKONG_GAMEACTOR_H_
#define _DONKEYKONG_GAMEACTOR_H_

#include "GameObject.h"
#include "GameProp.h"
#include "pixiretro/math.h"

class GameActor : public GameObject
{
public:


  virtual void onInteraction(const std::vector<GameProp>& interactions);

  inline bool isFalling() const;
  inline bool isJumping() const;
  inline bool isOnLadder() const;
  inline bool isNearLadder() const;

private:
  bool _isFalling;
  bool _isJumping;
  bool _isOnLadder;
  bool _isNearLadder;
};

#endif
