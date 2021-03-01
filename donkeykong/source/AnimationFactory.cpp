#include <string.h>
#include <cassert>
#include "pixiretro/pxr_xml.h"
#include "pixiretro/pxr_gfx.h"
#include "pixiretro/pxr_log.h"
#include "AnimationFactory.h"
#include "Animation.h"

using namespace tinyxml2;

std::unique_ptr<AnimationFactory> AnimationFactory::instance {nullptr};

//
// log strings.
//
static constexpr const char* msg_load_start = "loading animation definitions file";
static constexpr const char* msg_load_abort = "aborting animation definitions file load due to error";
static constexpr const char* msg_invalid_animation_mode = "invalid animation mode";
static constexpr const char* msg_use_animation_mode_default = "using default animation mode";
static constexpr const char* msg_missing_animation = "missing animation";

bool AnimationFactory::initialize()
{
  if(instance != nullptr)
    return true;

  instance = std::unique_ptr<AnimationFactory>{new AnimationFactory()};
  assert(instance != nullptr);
  return instance->loadAnimationDefinitions();
}

void AnimationFactory::shutdown()
{
  if(instance == nullptr)
    return;

  for(auto& pair : instance->_defs){
    pxr::gfx::unloadSpritesheet(pair.second->_spritesheetKey);
    pair.second.reset();
  }

  instance.reset(); 
}

Animation AnimationFactory::makeAnimation(const std::string& animationName)
{
  assert(instance != nullptr);
  auto search = instance->_defs.find(animationName);
  if(search == instance->_defs.end()){
    pxr::log::log(pxr::log::ERROR, msg_missing_animation, animationName);
    assert(0);
  }
  return Animation{search->second};
}

bool AnimationFactory::loadAnimationDefinitions()
{
  assert(_defs.size() == 0);

  std::string xmlpath {};
  xmlpath += ANIMATION_DEFINITIONS_FILE_PATH;
  xmlpath += ANIMATION_DEFINITIONS_FILE_NAME;
  xmlpath += pxr::io::XML_FILE_EXTENSION;

  pxr::log::log(pxr::log::INFO, msg_load_start, xmlpath);

  auto onerror = [](){
    pxr::log::log(pxr::log::ERROR, msg_load_abort);
    return false;
  };

  XMLDocument doc {};
  if(!pxr::io::parseXmlDocument(&doc, xmlpath))
    return onerror();

  XMLElement* xmlanimations {nullptr};
  XMLElement* xmlanimation {nullptr};
  XMLElement* xmlframe {nullptr};

  if(!pxr::io::extractChildElement(&doc, &xmlanimations, "animations")) 
    return onerror();

  if(!pxr::io::extractChildElement(xmlanimations, &xmlanimation, "animation")) 
    return onerror();

  do {
    const char* animationName {nullptr};
    if(!pxr::io::extractStringAttribute(xmlanimation, "name", &animationName)) return onerror();

    const char* spritesheetName {nullptr};
    if(!pxr::io::extractStringAttribute(xmlanimation, "spritesheet", &spritesheetName)) return onerror();
    pxr::gfx::ResourceKey_t spritesheetKey = pxr::gfx::loadSpritesheet(spritesheetName);

    float frequency {0.f};
    if(!pxr::io::extractFloatAttribute(xmlanimation, "frequency", &frequency)) return onerror();

    const char* modeName {nullptr};
    if(!pxr::io::extractStringAttribute(xmlanimation, "mode", &modeName)) return onerror();
    Animation::Mode animationMode;
    if(std::strcmp(modeName, "static") == 0)
      animationMode = Animation::Mode::STATIC;
    else if(std::strcmp(modeName, "forward") == 0)
      animationMode = Animation::Mode::FORWARD;
    else if(std::strcmp(modeName, "backward") == 0)
      animationMode = Animation::Mode::BACKWARD;
    else if(std::strcmp(modeName, "random") == 0)
      animationMode = Animation::Mode::RANDOM;
    else {
      pxr::log::log(pxr::log::ERROR, msg_invalid_animation_mode, modeName);
      pxr::log::log(pxr::log::ERROR, msg_use_animation_mode_default, "forward");
      animationMode = Animation::Mode::FORWARD;
    }

    int baseMirrorX{0}, baseMirrorY{0};
    if(!pxr::io::extractIntAttribute(xmlanimation, "mirrorX", &baseMirrorX)) return onerror();
    if(!pxr::io::extractIntAttribute(xmlanimation, "mirrorY", &baseMirrorY)) return onerror();

    if(!pxr::io::extractChildElement(xmlanimation, &xmlframe, "frame")) 
      return onerror();

    std::vector<pxr::gfx::SpriteId_t> frames {};

    do {
      pxr::gfx::SpriteId_t spriteid {0};
      if(!pxr::io::extractIntAttribute(xmlframe, "spriteid", &spriteid)) return onerror();
      frames.push_back(spriteid);
      xmlframe = xmlframe->NextSiblingElement("frame");
    }
    while(xmlframe != 0);

    std::shared_ptr<Animation::Definition> def = std::make_shared<Animation::Definition>(
      std::string{animationName},
      animationMode,
      spritesheetKey,
      std::move(frames),
      frequency,
      static_cast<bool>(baseMirrorX),
      static_cast<bool>(baseMirrorY)
    );

    _defs.emplace(std::make_pair(
      std::string{animationName},
      def
    ));

    xmlanimation = xmlanimation->NextSiblingElement("animation");
  }
  while(xmlanimation != 0);

  return true;
}

