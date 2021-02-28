#include <algorithm>
#include <string>
#include <vector>
#include <cassert>
#include "pixiretro/pxr_vec.h"
#include "pixiretro/pxr_xml.h"
#include "pixiretro/pxr_log.h"
#include "PropFactory.h"
#include "MarioFactory.h"
#include "Level.h"

using namespace tinyxml2;

//
// log strings.
//
static constexpr const char* msg_load_level = "loading level";
static constexpr const char* msg_load_success = "success loading level";
static constexpr const char* msg_load_abort = "aborting level load due to error";

Level::Level() :
  _state{STATE_UNLOADED},
  _controlScheme{nullptr},
  _props{},
  _marioSpawnPosition{0.f, 0.f},
  _mario{nullptr}
{}

bool Level::load(const std::string& file)
{
  assert(_state == STATE_UNLOADED);

  std::string xmlpath {};
  xmlpath += RESOURCE_PATH_LEVEL;
  xmlpath += file;
  xmlpath += pxr::io::XML_FILE_EXTENSION;

  pxr::log::log(pxr::log::INFO, msg_load_level, xmlpath);

  auto onerror = [](){
    pxr::log::log(pxr::log::INFO, msg_load_level);
    return false;
  };

  XMLDocument doc {};
  if(!pxr::io::parseXmlDocument(&doc, xmlpath))
    return onerror();

  XMLElement* xmllevel {nullptr};
  XMLElement* xmlmariospawn {nullptr};
  XMLElement* xmlprops {nullptr};
  XMLElement* xmlprop {nullptr};

  if(!pxr::io::extractChildElement(&doc, &xmllevel, "level"))
    return onerror();

  if(!pxr::io::extractChildElement(xmllevel, &xmlmariospawn, "marioSpawn"))
    return onerror();
    
  if(!pxr::io::extractFloatAttribute(xmlmariospawn, "x", &_marioSpawnPosition._x)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlmariospawn, "y", &_marioSpawnPosition._y)) return onerror();
  
  if(!pxr::io::extractChildElement(xmllevel, &xmlprops, "props"))
    return onerror();

  if(!pxr::io::extractChildElement(xmlprops, &xmlprop, "prop"))
    return onerror();

  do {
    const char* propName {nullptr};
    if(!pxr::io::extractStringAttribute(xmlprop, "name", &propName)) return onerror();

    pxr::Vector2f position {};
    if(!pxr::io::extractFloatAttribute(xmlprop, "x", &position._x)) return onerror();
    if(!pxr::io::extractFloatAttribute(xmlprop, "y", &position._y)) return onerror();

    _props.emplace_back(std::move(PropFactory::makeProp(position, propName)));

    xmlprop = xmlprop->NextSiblingElement("prop");
  }
  while(xmlprop != 0);

  auto compare = [](const Prop& p0, const Prop& p1){
    return p0.getDrawLayer() < p1.getDrawLayer();
  };

  //
  // sort in ascending order of draw layer so we draw in the correct order.
  //
  std::sort(_props.begin(), _props.end(), compare);

  _state = STATE_UNINITIALIZED;

  pxr::log::log(pxr::log::INFO, msg_load_success, xmlpath);

  return true;
}

void Level::onInit(std::shared_ptr<const ControlScheme> controlScheme)
{
  assert(_state == STATE_UNINITIALIZED);

  _controlScheme = std::move(controlScheme);

  if(_mario == nullptr){
    _mario = std::unique_ptr<Mario>{new Mario{std::move(MarioFactory::makeMario(
      _marioSpawnPosition,
      _controlScheme
    ))}};
  }

  changeState(STATE_PLAYING); // TODO TEMP - implement cutscenes
}

void Level::onUpdate(double now, float dt)
{
  assert(0 <= _state && _state < STATE_COUNT);

  for(auto& prop : _props)
    prop.onUpdate(now, dt);

  _mario->onInput();
  _mario->onUpdate(now, dt);

  //
  // handle collisions/interactions.
  //
}

void Level::onDraw(int screenid)
{
  assert(0 <= _state && _state < STATE_COUNT);

  for(auto& prop : _props)
    prop.onDraw(screenid);

  _mario->onDraw(screenid);
}

void Level::reset()
{
  assert(0 <= _state && _state < STATE_COUNT);

  for(auto& prop : _props)
    prop.reset();

  changeState(STATE_PLAYING); // TODO TEMP - implement cutscenes
}

void Level::changeState(State state)
{
  switch(_state){
    case STATE_ENTRANCE_CUTSCENE:
      endEntranceCutscene();
      break;
    case STATE_PLAYING:
      endPlaying();
      break;
    case STATE_EXIT_CUTSCENE:
      endExitCutscene();
      break;
    default:
      break;
  }

  _state = state;

  switch(_state){
    case STATE_ENTRANCE_CUTSCENE:
      startEntranceCutscene();
      break;
    case STATE_PLAYING:
      startPlaying();
      break;
    case STATE_EXIT_CUTSCENE:
      startExitCutscene();
      break;
    default:
      assert(0);
  }
}

void Level::startEntranceCutscene()
{

}

void Level::endEntranceCutscene()
{
}

void Level::startPlaying()
{
  _mario->respawn();
}

void Level::endPlaying()
{
}

void Level::startExitCutscene()
{
}

void Level::endExitCutscene()
{
}

void Level::updateEntranceCutscene(double now, float dt)
{
}

void Level::updatePlaying(double now, float dt)
{
}

void Level::updateExitCutscene(double now, float dt)
{
}

