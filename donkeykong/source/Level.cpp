#include <algorithm>
#include <string>
#include <vector>
#include "pixiretro/pxr_vec.h"
#include "pixiretro/pxr_xml.h"
#include "Level.h"
#include "GamePropFactory.h"

using namespace tinyxml2;

//
// log strings.
//
static constexpr const char* msg_load_level = "loading level";
static constexpr const char* msg_load_success = "success loading level";
static constexpr const char* msg_load_abort = "aborting level load due to error";

bool Level::load(const std::string& file)
{
  _props.clear();
  _mario.reset();

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

  if(!pxr::io::extractChildElement(&doc, &xmlmariospawn, "marioSpawn"))
    return onerror();
    
  if(!pxr::io::extractFloatAttribute(xmlmariospawn, "x", &marioSpawn._x)) return onerror();
  if(!pxr::io::extractFloatAttribute(xmlmariospawn, "y", &marioSpawn._y)) return onerror();
  
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

    _props.emplace_back(std::move(GamePropFactory::makeGameProp(position, propName)));

    xmlprop = xmlprop->NextSiblingElement("prop");
  }
  while(xmlprop != 0);

  auto compare = [](const GameProp& p0, const GameProp& p1){
    return p0.getDrawLayer() < p1.getDrawLayer();
  };

  //
  // sort in ascending order of draw layer so we draw in the correct order.
  //
  std::sort(_props.begin(), _props.end(), compare);

  pxr::log::log(pxr::log::INFO, msg_load_success, xmlpath);
}

void Level::onUpdate(double now, float dt)
{
  for(auto& prop : _props)
    prop.onUpdate(now, dt);
}

void Level::onDraw(int screenid)
{
  for(auto& prop : _props)
    prop.onDraw(screenid);
}

void Level::reset()
{
  for(auto& prop : _props)
    prop.reset();

  _mario->reset();
}

void Level::switchState(State state)
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
  }
}

void Level::startEntranceCutscene()
{

}

void Level::endEntraceCutscene()
{
}

void Level::startPlaying()
{
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

void Level::updateEntraceCutscene(double now, float dt)
{
}

void Level::updatePlaying(double now, float dt)
{
}

void Level::updateExitCutscene(double now, float dt)
{
}

