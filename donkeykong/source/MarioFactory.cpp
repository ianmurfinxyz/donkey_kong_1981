#include <array>
#include <memory>
#include <cstring>
#include <cassert>

#include "pixiretro/pxr_xml.h"
#include "pixiretro/pxr_log.h"
#include "pixiretro/pxr_rect.h"
#include "MarioFactory.h"

using namespace tinyxml2;

std::unique_ptr<MarioFactory> MarioFactory::instance {nullptr};

//
// log strings.
//
static constexpr const char* msg_load_start = "loading mario definition file";
static constexpr const char* msg_load_abort = "aborting mario definition load due to error";
static constexpr const char* msg_load_success = "success loading mario definition file";

bool MarioFactory::initialize()
{
  if(instance != nullptr)
    return true;

  instance = std::unique_ptr<MarioFactory>{new MarioFactory()};
  assert(instance != nullptr);
  return instance->loadMarioDefinition();
}

void MarioFactory::shutdown()
{
  if(instance == nullptr)
    return;

  for(auto& pair : instance->_marioDefinition->_sounds)
    if(pair.first != -1)
      pxr::sfx::unloadSound(pair.first);

  instance.reset();
}

Mario MarioFactory::makeMario(pxr::Vector2f spawnPosition, std::shared_ptr<const ControlScheme> controlScheme)
{
  assert(instance != nullptr);
  return Mario(spawnPosition, controlScheme, instance->_marioDefinition); 
}

bool MarioFactory::loadMarioDefinition()
{
  assert(_marioDefinition == nullptr); 

  std::string xmlpath {};
  xmlpath += MARIO_DEFINITION_FILE_PATH;
  xmlpath += MARIO_DEFINITION_FILE_NAME;
  xmlpath += pxr::io::XML_FILE_EXTENSION;

  pxr::log::log(pxr::log::INFO, msg_load_start, xmlpath);

  auto onerror = [](){
    pxr::log::log(pxr::log::ERROR, msg_load_abort);
    return false;
  };

  XMLDocument doc {};
  if(!pxr::io::parseXmlDocument(&doc, xmlpath))
    return onerror();

  XMLElement* xmlmario {nullptr};
  XMLElement* xmlanimations {nullptr};
  XMLElement* xmlsounds {nullptr};
  XMLElement* xmlpropbox {nullptr};

  if(!pxr::io::extractChildElement(&doc, &xmlmario, "mario"))
    return onerror();

  float runSpeed, climbSpeed, jumpImpulse, jumpDuration, gravity, maxFall, spawnDuration, 
        dyingDuration;
  int marioW, marioH, spawnHealth;

  if(!pxr::io::extractIntAttribute(xmlmario, "width", &marioW)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlmario, "height", &marioH)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlmario, "runSpeed", &runSpeed)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlmario, "climbSpeed", &climbSpeed)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlmario, "jumpImpulse", &jumpImpulse)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlmario, "jumpDuration", &jumpDuration)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlmario, "gravity", &gravity)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlmario, "maxFall", &maxFall)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlmario, "spawnHealth", &spawnHealth)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlmario, "spawnDuration", &spawnDuration)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlmario, "dyingDuration", &dyingDuration)) return onerror();

  const char* cstr {nullptr};

  std::array<std::string, Mario::STATE_COUNT> animationNames;

  if(!pxr::io::extractChildElement(xmlmario, &xmlanimations, "animations"))
    return onerror();

  if(!pxr::io::extractStringAttribute(xmlanimations, "idle", &cstr)) return onerror();
  animationNames[Mario::STATE_IDLE] = std::string{cstr};
  if(!pxr::io::extractStringAttribute(xmlanimations, "run", &cstr)) return onerror();
  animationNames[Mario::STATE_RUNNING] = std::string{cstr};
  if(!pxr::io::extractStringAttribute(xmlanimations, "climbUp", &cstr)) return onerror();
  animationNames[Mario::STATE_CLIMBING_UP] = std::string{cstr};
  if(!pxr::io::extractStringAttribute(xmlanimations, "climbDown", &cstr)) return onerror();
  animationNames[Mario::STATE_CLIMBING_DOWN] = std::string{cstr};
  if(!pxr::io::extractStringAttribute(xmlanimations, "jump", &cstr)) return onerror();
  animationNames[Mario::STATE_JUMPING] = std::string{cstr};
  if(!pxr::io::extractStringAttribute(xmlanimations, "fall", &cstr)) return onerror();
  animationNames[Mario::STATE_FALLING] = std::string{cstr};
  if(!pxr::io::extractStringAttribute(xmlanimations, "spawn", &cstr)) return onerror();
  animationNames[Mario::STATE_SPAWNING] = std::string{cstr};
  if(!pxr::io::extractStringAttribute(xmlanimations, "die", &cstr)) return onerror();
  animationNames[Mario::STATE_DYING] = std::string{cstr};

  int loop {false};
  std::array<std::pair<pxr::sfx::ResourceKey_t, bool>, Mario::STATE_COUNT> sounds;

  if(!pxr::io::extractChildElement(xmlmario, &xmlsounds, "sounds"))
    return onerror();

  if(!pxr::io::extractStringAttribute(xmlsounds, "idle", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "idleLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[Mario::STATE_IDLE].first = -1;
  else
    sounds[Mario::STATE_IDLE].first = pxr::sfx::loadSound(cstr);
  sounds[Mario::STATE_IDLE].second = static_cast<bool>(loop);

  if(!pxr::io::extractStringAttribute(xmlsounds, "run", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "runLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[Mario::STATE_RUNNING].first = -1;
  else
    sounds[Mario::STATE_RUNNING].first = pxr::sfx::loadSound(cstr);
  sounds[Mario::STATE_RUNNING].second = static_cast<bool>(loop);

  if(!pxr::io::extractStringAttribute(xmlsounds, "climbUp", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "climbUpLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[Mario::STATE_CLIMBING_UP].first = -1;
  else
    sounds[Mario::STATE_CLIMBING_UP].first = pxr::sfx::loadSound(cstr);
  sounds[Mario::STATE_CLIMBING_UP].second = static_cast<bool>(loop);

  if(!pxr::io::extractStringAttribute(xmlsounds, "climbDown", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "climbDownLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[Mario::STATE_CLIMBING_DOWN].first = -1;
  else
    sounds[Mario::STATE_CLIMBING_DOWN].first = pxr::sfx::loadSound(cstr);
  sounds[Mario::STATE_CLIMBING_DOWN].second = static_cast<bool>(loop);

  if(!pxr::io::extractStringAttribute(xmlsounds, "jump", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "jumpLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[Mario::STATE_JUMPING].first = -1;
  else
    sounds[Mario::STATE_JUMPING].first = pxr::sfx::loadSound(cstr);
  sounds[Mario::STATE_JUMPING].second = static_cast<bool>(loop);

  if(!pxr::io::extractStringAttribute(xmlsounds, "fall", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "fallLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[Mario::STATE_FALLING].first = -1;
  else
    sounds[Mario::STATE_FALLING].first = pxr::sfx::loadSound(cstr);
  sounds[Mario::STATE_FALLING].second = static_cast<bool>(loop);

  if(!pxr::io::extractStringAttribute(xmlsounds, "die", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "dieLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[Mario::STATE_DYING].first = -1;
  else
    sounds[Mario::STATE_DYING].first = pxr::sfx::loadSound(cstr);
  sounds[Mario::STATE_DYING].second = static_cast<bool>(loop);

  if(!pxr::io::extractChildElement(xmlmario, &xmlpropbox, "propBox"))
    return onerror();

  pxr::fRect propBox {};

  if(!pxr::io::extractFloatAttribute(xmlpropbox, "x", &propBox._x)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlpropbox, "y", &propBox._y)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlpropbox, "width", &propBox._w)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlpropbox, "height", &propBox._h)) return onerror();

  _marioDefinition = std::shared_ptr<Mario::Definition>{new Mario::Definition(
    std::move(animationNames),
    std::move(sounds),
    pxr::Vector2i{marioW, marioH},
    propBox,
    runSpeed,
    climbSpeed,
    jumpImpulse,
    jumpDuration,
    gravity,
    maxFall,
    spawnHealth,
    spawnDuration,
    dyingDuration
  )};

  pxr::log::log(pxr::log::INFO, msg_load_success);

  return true;
}
