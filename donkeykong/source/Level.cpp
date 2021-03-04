#include <algorithm>
#include <string>
#include <vector>
#include <cassert>
#include "pixiretro/pxr_vec.h"
#include "pixiretro/pxr_xml.h"
#include "pixiretro/pxr_log.h"
#include "pixiretro/pxr_collision.h"
#include "PropFactory.h"
#include "Prop.h"
#include "MarioFactory.h"
#include "PlayState.h"
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
  _ending{ENDING_NONE},
  _controlScheme{nullptr},
  _props{},
  _marioSpawnPosition{0.f, 0.f},
  _mario{nullptr},
  _propInteractions{},
  _isDebugDraw{false}
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

void Level::unload()
{
  _controlScheme.reset();
  _props.clear();
  _propInteractions.clear();
  _marioSpawnPosition.zero();
  _mario.reset();
  _isDebugDraw = false;
  _state = STATE_UNLOADED;
  _ending = ENDING_NONE;
}

void Level::onInit(std::shared_ptr<const ControlScheme> controlScheme)
{
  assert(_state == STATE_UNINITIALIZED);

  _controlScheme = controlScheme;

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

  if(_state == STATE_OVER)
    return;

  switch(_state){
    case STATE_ENTRANCE_CUTSCENE:
      updateEntranceCutscene(now, dt);
      break;
    case STATE_PLAYING:
      updatePlaying(now, dt);
      break;
    case STATE_EXIT_CUTSCENE:
      updateExitCutscene(now, dt);
      break;
    default:
      break;
  }

  if(pxr::input::isKeyPressed(debugDrawToggleKey))
    _isDebugDraw = !_isDebugDraw;
}

void Level::onDraw(int screenid)
{
  assert(0 <= _state && _state < STATE_COUNT);

  for(auto& prop : _props)
    prop.onDraw(screenid);

  _mario->onDraw(screenid);

  if(_isDebugDraw)
    debugDraw(screenid);
}

void Level::reset()
{
  assert(0 <= _state && _state < STATE_COUNT);

  for(auto& prop : _props)
    prop.reset();

  changeState(STATE_PLAYING); // TODO TEMP - implement cutscenes
}

bool Level::isOver()
{
  if(_state == STATE_OVER){
    assert(_ending != ENDING_NONE);
    return true;
  }
  return false;
}

Level::Ending Level::getEnding()
{
  return _ending;
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
    case STATE_OVER:
      startOverState();
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

void Level::startOverState()
{
  _ending = _mario->isDead() ? ENDING_LOSS : ENDING_WIN;
}

void Level::updateEntranceCutscene(double now, float dt)
{
}

void Level::updatePlaying(double now, float dt)
{
  static pxr::CollisionSubject subjectA, subjectB;

  if(_mario->isDying() && _isMusicPlaying)
    stopMusic();

  if(_mario->isDead()){
    changeState(STATE_OVER);
    return;
  }

  for(auto& prop : _props)
    prop.onUpdate(now, dt);

  _propInteractions.clear();
  for(auto& prop : _props){
    if(pxr::isAABBIntersection(prop.getInteractionBox(), _mario->getPropInteractionBox())){
      if(prop.isKiller()){
        subjectA._position = _mario->getPosition();
        subjectA._spritesheetKey = _mario->getSpritesheetKey();
        subjectA._spriteid = _mario->getSpriteId();
        subjectB._position = prop.getPosition();
        subjectB._spritesheetKey = prop.getSpritesheetKey();
        subjectB._spriteid = prop.getSpriteId();
        const pxr::CollisionResult& result = pxr::isPixelIntersection(subjectA, subjectB);
        if(!result._isCollision)
          continue;
      }
      _propInteractions.push_back(&prop);
    }
  }
  _mario->onPropInteractions(_propInteractions);

  _mario->onInput();
  _mario->onUpdate(now, dt);

}

void Level::updateExitCutscene(double now, float dt)
{
}

void Level::debugDraw(int screenid)
{
  pxr::iRect rect;
  for(const auto& prop : _props){
    const pxr::AABB& aabb = prop.getInteractionBox();
    rect._x = aabb._xmin;
    rect._y = aabb._ymin;
    rect._w = aabb._xmax - aabb._xmin;
    rect._h = aabb._ymax - aabb._ymin;
    pxr::gfx::drawBorderRectangle(rect, pxr::gfx::colors::green, screenid);
  }

  const pxr::AABB& aabb = _mario->getPropInteractionBox();
  rect._x = aabb._xmin;
  rect._y = aabb._ymin;
  rect._w = aabb._xmax - aabb._xmin;
  rect._h = aabb._ymax - aabb._ymin;
  pxr::gfx::drawBorderRectangle(rect, pxr::gfx::colors::yellow, screenid);
}

void Level::startMusic()
{

}

void Level::stopMusic()
{

}

