#include <array>
#include <memory>
#include <cstring>

#include "pixiretro/pxr_xml.h"
#include "MarioFactory.h"

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

void MarioFactory::makeMario(pxr::Vector2f position, std::shared_ptr<const ControlScheme> controlScheme)
{
  assert(instance != nullptr);
  return Mario(position, std::move(controlScheme), _marioDefinition); 
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
  XMLElement* xmlmovespeed {nullptr};
  XMLElement* xmlclimbspeed {nullptr};
  XMLElement* xmlfallspeed {nullptr};
  XMLElement* xmljumpspeed {nullptr};
  XMLElement* xmlspawnhealth {nullptr};

  if(!pxr::io::extractChildElement(&doc, &xmlmario, "mario"))
    return onerror();

  float moveSpeed, climbSpeed, fallSpeed, jumpSpeed, spawnHealth;
  int spawnHealth;

  if(!pxr::io::extractFloatAttribute(xmlmario, "moveSpeed", &moveSpeed)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlmario, "climbSpeed", &climbSpeed)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlmario, "fallSpeed", &fallSpeed)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlmario, "jumpSpeed", &jumpSpeed)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlmario, "spawnHealth", &spawnHealth)) return onerror();

  const char* cstr {nullptr};

  std::array<std::string, mario::STATE_COUNT> animationNames;

  if(!pxr::io::extractChildElement(xmlmario, &xmlanimations, "animations"))
    return onerror();

  if(!pxr::io::extractStringAttribute(xmlanimations, "idle", &cstr)) return onerror();
  animationNames[mario::STATE_IDLE] = std::string{cstr};
  if(!pxr::io::extractStringAttribute(xmlanimations, "moveLeft", &cstr)) return onerror();
  animationNames[mario::STATE_MOVE_LEFT] = std::string{cstr};
  if(!pxr::io::extractStringAttribute(xmlanimations, "moveRight", &cstr)) return onerror();
  animationNames[mario::STATE_MOVE_RIGHT] = std::string{cstr};
  if(!pxr::io::extractStringAttribute(xmlanimations, "climbUp", &cstr)) return onerror();
  animationNames[mario::STATE_CLIMBING_UP] = std::string{cstr};
  if(!pxr::io::extractStringAttribute(xmlanimations, "climbDown", &cstr)) return onerror();
  animationNames[mario::STATE_CLIMBING_DOWN] = std::string{cstr};
  if(!pxr::io::extractStringAttribute(xmlanimations, "jump", &cstr)) return onerror();
  animationNames[mario::STATE_JUMPING] = std::string{cstr};
  if(!pxr::io::extractStringAttribute(xmlanimations, "fall", &cstr)) return onerror();
  animationNames[mario::STATE_FALLING] = std::string{cstr};
  if(!pxr::io::extractStringAttribute(xmlanimations, "die", &cstr)) return onerror();
  animationNames[mario::STATE_DYING] = std::string{cstr};

  int loop {false};
  std::array<std::pair<pxr::sfx::ResourceKey_t, bool>, mario::STATE_COUNT> sounds;

  if(!pxr::io::extractChildElement(xmlmario, &xmlsounds, "sounds"))
    return onerror();

  if(!pxr::io::extractStringAttribute(xmlsounds, "idle", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "idleLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[mario::STATE_IDLE].first = -1;
  else
    sounds[mario::STATE_IDLE].first = pxr::sfx::loadSound(cstr);
  sounds[mario::STATE_IDLE].second = static_cast<bool>(loop);

  if(!pxr::io::extractStringAttribute(xmlsounds, "moveLeft", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "moveLeftLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[mario::STATE_MOVING_LEFT].first = -1;
  else
    sounds[mario::STATE_MOVING_LEFT].first = pxr::sfx::loadSound(cstr);
  sounds[mario::STATE_MOVING_LEFT].second = static_cast<bool>(loop);

  if(!pxr::io::extractStringAttribute(xmlsounds, "moveRight", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "moveRightLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[mario::STATE_MOVING_RIGHT].first = -1;
  else
    sounds[mario::STATE_MOVING_RIGHT].first = pxr::sfx::loadSound(cstr);
  sounds[mario::STATE_MOVING_RIGHT].second = static_cast<bool>(loop);

  if(!pxr::io::extractStringAttribute(xmlsounds, "clmibUp", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "clmibUpLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[mario::STATE_CLIMBING_UP].first = -1;
  else
    sounds[mario::STATE_CLIMBING_UP].first = pxr::sfx::loadSound(cstr);
  sounds[mario::STATE_CLIMBING_UP].second = static_cast<bool>(loop);

  if(!pxr::io::extractStringAttribute(xmlsounds, "clmibDown", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "clmibDownLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[mario::STATE_CLIMBING_DOWN].first = -1;
  else
    sounds[mario::STATE_CLIMBING_DOWN].first = pxr::sfx::loadSound(cstr);
  sounds[mario::STATE_CLIMBING_DOWN].second = static_cast<bool>(loop);

  if(!pxr::io::extractStringAttribute(xmlsounds, "jump", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "jumpLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[mario::STATE_JUMPING].first = -1;
  else
    sounds[mario::STATE_JUMPING].first = pxr::sfx::loadSound(cstr);
  sounds[mario::STATE_JUMPING].second = static_cast<bool>(loop);

  if(!pxr::io::extractStringAttribute(xmlsounds, "fall", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "fallLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[mario::STATE_FALLING].first = -1;
  else
    sounds[mario::STATE_FALLING].first = pxr::sfx::loadSound(cstr);
  sounds[mario::STATE_FALLING].second = static_cast<bool>(loop);

  if(!pxr::io::extractStringAttribute(xmlsounds, "die", &cstr)) return onerror();
  if(!pxr::io::extractIntAttribute(xmlsounds, "dieLoop", &loop)) return onerror();
  if(std::strcmp(cstr, "NA") == 0 || std::strlen(cstr) == 0)
    sounds[mario::STATE_DYING].first = -1;
  else
    sounds[mario::STATE_DYING].first = pxr::sfx::loadSound(cstr);
  sounds[mario::STATE_DYING].second = static_cast<bool>(loop);

  _marioDefinition = std::unique_ptr<Mario::Definition>{new Mario::Definition(
    std::move(animationNames),
    std::move(sounds),
    moveSpeed,
    climbSpeed,
    fallSpeed,
    jumpSpeed,
    spawnHealth
  )};

  pxr::log::log(pxr::log::INFO, msg_load_success);

  return true;
}
