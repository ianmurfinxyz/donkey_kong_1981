#include "pixiretro/pxr_xml.h"
#include "AnimationFactory.h"
#include "Animation.h"

std::unique_ptr<AnimationFactory> AnimationFactory::instance;

bool AnimationFactory::initialize()
{
  if(instance != nullptr)
    return true;

  instance = std::make_unique<AnimationFactory>();
  assert(instance != nullptr);
  return instance->loadAnimationDefinitions();
}

void AnimationFactory::shutdown()
{
  instance.reset(nullptr); 
}

Animation AnimationFactory::makeAnimation(const std::string& animationName)
{
  assert(instance != nullptr);
  auto search = instance->_defs.find(animationName);
  assert(search != instance->_defs.end());
  return Animation{search->second};
}

bool AnimationFactory::loadAnimationDefinitions()
{
 // TODO
}

