#include "pixiretro/pxr_xml.h"
#include "AnimationFactory.h"
#include "Animation.h"


AnimationFactory::loadAnimationDefinitions()
{
 // TODO
}

Animation AnimationFactory::makeAnimation(const std::string& animationName)
{
  auto search = _defs.find(animationName);
  assert(search != _defs.end());
  return Animation{&(search->second));
}
