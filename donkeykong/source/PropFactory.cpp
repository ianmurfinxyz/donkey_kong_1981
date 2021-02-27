
#include <cstring>
#include <cassert>
#include "PropFactory.h"
#include "pixiretro/pxr_log.h"
#include "pixiretro/pxr_xml.h"

using namespace tinyxml2;

std::unique_ptr<PropFactory> PropFactory::instance {nullptr};

//
// log strings.
//
static constexpr const char* msg_load_start = "loading prop definitions file";
static constexpr const char* msg_load_abort = "aborting prop definitions load due to error";
static constexpr const char* msg_load_success = "success loading prop definitions file";
static constexpr const char* msg_empty_prop_name = "read empty prop name";
static constexpr const char* msg_empty_sound_name = "read empty sound name";

bool PropFactory::initialize()
{
  if(instance != nullptr)
    return true;

  instance = std::unique_ptr<PropFactory>{new PropFactory()};
  assert(instance != nullptr);
  return instance->loadPropDefinitions();
}

void PropFactory::shutdown()
{
  if(instance == nullptr)
    return;

  for(auto& pair : instance->_defs)
    for(auto& state : pair.second->_states)
      for(auto& sound : state._sounds)
        pxr::sfx::unloadSound(sound);

  instance.reset();
}

Prop PropFactory::makeProp(pxr::Vector2f position, const std::string& propName)
{
  assert(instance != nullptr);
  auto search = instance->_defs.find(propName);
  assert(search != instance->_defs.end());
  return Prop{position, search->second};
}

bool PropFactory::loadPropDefinitions()
{
  assert(_defs.size() == 0);

  std::string xmlpath {};
  xmlpath += PROP_DEFINITIONS_FILE_PATH;
  xmlpath += PROP_DEFINITIONS_FILE_NAME;
  xmlpath += pxr::io::XML_FILE_EXTENSION;

  pxr::log::log(pxr::log::INFO, msg_load_start, xmlpath);

  auto onerror = [](){
    pxr::log::log(pxr::log::ERROR, msg_load_abort);
    return false;
  };

  XMLDocument doc {};
  if(!pxr::io::parseXmlDocument(&doc, xmlpath))
    return onerror();

  XMLElement* xmlprop {nullptr};
  XMLElement* xmlstate {nullptr};
  XMLElement* xmltransition {nullptr};
  XMLElement* xmlpositions {nullptr};
  XMLElement* xmlspeeds {nullptr};
  XMLElement* xmlpoint {nullptr};
  XMLElement* xmlbox {nullptr};
  XMLElement* xmleffects {nullptr};
  XMLElement* xmlsupport {nullptr};
  XMLElement* xmlladder {nullptr};
  XMLElement* xmlconveyor {nullptr};
  XMLElement* xmlkiller {nullptr};
  XMLElement* xmlsounds {nullptr};
  XMLElement* xmlsound {nullptr};
  XMLElement* xmlanimation {nullptr};

  ///////////////////////////////////////////////////////////////////////////////////////////////
  // LOAD EACH PROP DEFINITION 
  ///////////////////////////////////////////////////////////////////////////////////////////////

  if(!pxr::io::extractChildElement(&doc, &xmlprop, "prop")) 
    return onerror();

  do {
    
    const char* propName;
    if(!pxr::io::extractStringAttribute(xmlprop, "name", &propName)) return onerror();
    if(std::strcmp(propName, "") == 0){
      pxr::log::log(pxr::log::ERROR, msg_empty_prop_name);
      return onerror();
    };

    Prop::StateTransitionMode tmode;
    const char* tsmode;
    if(!pxr::io::extractStringAttribute(xmlprop, "stateTransitionMode", &tsmode)) return onerror();
    if(std::strcmp(tsmode, "random") == 0)
      tmode = Prop::StateTransitionMode::RANDOM;
    else if(std::strcmp(tsmode, "forward") == 0)
      tmode = Prop::StateTransitionMode::FORWARD;
    else
      return onerror();

    int drawLayer;
    if(!pxr::io::extractIntAttribute(xmlprop, "drawLayer", &drawLayer)) return onerror();

    /////////////////////////////////////////////////////////////////////////////////////////////
    // LOAD EACH STATE DEFINITION IN THE PROP DEFINITION
    /////////////////////////////////////////////////////////////////////////////////////////////
    
    std::vector<Prop::StateDefinition> states;

    if(!pxr::io::extractChildElement(xmlprop, &xmlstate, "state")) return onerror();

    do {
      float stateDuration;
      if(!pxr::io::extractFloatAttribute(xmlstate, "duration", &stateDuration))
        return onerror();

      //
      // Extract state transition.
      //
      
      std::shared_ptr<std::vector<pxr::Vector2f>> positions = 
        std::make_shared<std::vector<pxr::Vector2f>>();

      if(!pxr::io::extractChildElement(xmlstate, &xmltransition, "transition")) return onerror();
      if(!pxr::io::extractChildElement(xmltransition, &xmlpositions, "positions")) return onerror();
      if(!pxr::io::extractChildElement(xmlpositions, &xmlpoint, "point")) return onerror();
      do {
        float x, y;
        if(!pxr::io::extractFloatAttribute(xmlpoint, "x", &x)) return onerror();
        if(!pxr::io::extractFloatAttribute(xmlpoint, "y", &y)) return onerror();
        positions->push_back(pxr::Vector2f{x, y});
        xmlpoint = xmlpoint->NextSiblingElement("point");
      }
      while(xmlpoint != 0);

      std::shared_ptr<std::vector<Transition::SpeedPoint>> speeds = 
        std::make_shared<std::vector<Transition::SpeedPoint>>();

      if(!pxr::io::extractChildElement(xmltransition, &xmlspeeds, "speeds")) return onerror();
      if(!pxr::io::extractChildElement(xmlspeeds, &xmlpoint, "point")) return onerror();
      do {
        float value, duration;
        if(!pxr::io::extractFloatAttribute(xmlpoint, "value", &value)) return onerror();
        if(!pxr::io::extractFloatAttribute(xmlpoint, "duration", &duration)) return onerror();
        speeds->push_back(Transition::SpeedPoint{value, duration});
        xmlpoint = xmlpoint->NextSiblingElement("point");
      }
      while(xmlpoint != 0);

      //
      // Extract interaction box.
      //

      pxr::fRect interactionBox {};
      if(!pxr::io::extractChildElement(xmlstate, &xmlbox, "interactionBox")) return onerror();
      if(!pxr::io::extractFloatAttribute(xmlbox, "x", &interactionBox._x)) return onerror();
      if(!pxr::io::extractFloatAttribute(xmlbox, "y", &interactionBox._y)) return onerror();
      if(!pxr::io::extractFloatAttribute(xmlbox, "width", &interactionBox._w)) return onerror();
      if(!pxr::io::extractFloatAttribute(xmlbox, "height", &interactionBox._h)) return onerror();

      //
      // Extract state effects.
      //

      if(!pxr::io::extractChildElement(xmlstate, &xmleffects, "effects")) return onerror();

      int isSupport;
      float supportHeight;
      if(!pxr::io::extractChildElement(xmleffects, &xmlsupport, "support")) return onerror();
      if(!pxr::io::extractIntAttribute(xmlsupport, "active", &isSupport)) return onerror();
      if(!pxr::io::extractFloatAttribute(xmlsupport, "height", &supportHeight)) return onerror();

      int isLadder;
      float ladderHeight;
      if(!pxr::io::extractChildElement(xmleffects, &xmlladder, "ladder")) return onerror();
      if(!pxr::io::extractIntAttribute(xmlladder, "active", &isLadder)) return onerror();
      if(!pxr::io::extractFloatAttribute(xmlladder, "height", &ladderHeight)) return onerror();
      
      int isConveyor;
      pxr::Vector2f conveyorVelocity {};
      if(!pxr::io::extractChildElement(xmleffects, &xmlconveyor, "conveyor")) return onerror();
      if(!pxr::io::extractIntAttribute(xmlconveyor, "active", &isConveyor)) return onerror();
      if(!pxr::io::extractFloatAttribute(xmlconveyor, "velocityX", &conveyorVelocity._x)) return onerror();
      if(!pxr::io::extractFloatAttribute(xmlconveyor, "velocityY", &conveyorVelocity._y)) return onerror();

      int isKiller;
      int killerDamage;
      if(!pxr::io::extractChildElement(xmleffects, &xmlkiller, "killer")) return onerror();
      if(!pxr::io::extractIntAttribute(xmlkiller, "active", &isKiller)) return onerror();
      if(!pxr::io::extractIntAttribute(xmlkiller, "damage", &killerDamage)) return onerror();

      //
      // Extract state sounds.
      //

      std::vector<pxr::sfx::ResourceKey_t> sounds {};
      if(!pxr::io::extractChildElement(xmlstate, &xmlsounds, "sounds")) return onerror();
      if(!pxr::io::extractChildElement(xmlsounds, &xmlsound, "sound")) return onerror();
      do {
        const char* soundName;
        if(!pxr::io::extractStringAttribute(xmlsound, "name", &soundName)) return onerror();
        if(std::strlen(soundName) == 0){
          pxr::log::log(pxr::log::WARN, msg_empty_sound_name, propName);
          continue;
        }
        if(std::strcmp(soundName, "NA") != 0){
          sounds.push_back(pxr::sfx::loadSound(soundName));
        }
        xmlsound = xmlsound->NextSiblingElement("sound");
      }
      while(xmlsound != 0);

      //
      // Extract state animation.
      //

      const char* animationName;
      if(!pxr::io::extractChildElement(xmlstate, &xmlanimation, "animation")) return onerror();
      if(!pxr::io::extractStringAttribute(xmlanimation, "name", &animationName)) return onerror();

      //
      // Construct the state instance.
      //
      states.emplace_back(
        positions,
        speeds,
        sounds,
        interactionBox,
        animationName,
        stateDuration,
        supportHeight,
        ladderHeight,
        conveyorVelocity,
        killerDamage,
        static_cast<bool>(isSupport),
        static_cast<bool>(isLadder),
        static_cast<bool>(isConveyor),
        static_cast<bool>(isKiller)
      );

      xmlstate = xmlstate->NextSiblingElement("state");
    }
    while(xmlstate != 0);

    //// STATE LOAD END /////////////////////////////////////////////////////////////////////////

    std::shared_ptr<Prop::Definition> def {new Prop::Definition{
      std::string{propName},
      tmode,
      std::move(states),
      drawLayer
    }};

    _defs.emplace(std::make_pair(
      std::string{propName},
      def
    ));

    xmlprop = xmlprop->NextSiblingElement("prop");
  }
  while(xmlprop != 0);

  //// PROP LOAD END ////////////////////////////////////////////////////////////////////////////
  
  pxr::log::log(pxr::log::INFO, msg_load_success);
  
  return true;
}

