#include "pixiretro/pxr_xml.h"
#include "AnimationFactory.h"
#include "Animation.h"

AnimationFactory* AnimationFactory::instance {nullptr};

bool AnimationFactory::initialize()
{
  instance = new AnimationFactory();
  assert(instance != nullptr);
  return instance->loadAnimationDefinitions();
}

Animation AnimationFactory::makeAnimation(const std::string& animationName)
{
  auto search = _defs.find(animationName);
  assert(search != _defs.end());
  return Animation{&(search->second));
}

bool AnimationFactory::loadAnimationDefinitions()
{
 // TODO
}

