#include <cassert>
#include <cstdlib>
#include "pixiretro/pxr_gfx.h"
#include "pixiretro/pxr_xml.h"
#include "pixiretro/pxr_log.h"
#include "PlayState.h"

#include <iostream>

using namespace tinyxml2;

static constexpr const char* msg_load_start {"loading dkconfig file"};
static constexpr const char* msg_load_abort {"aborting dkconfig load due to error"};
static constexpr const char* msg_load_success {"successfully loaded dkconfig file"};
static constexpr const char* msg_invalid_key {"invalid key string"};

PlayState::PlayState(pxr::App* owner) :
  pxr::AppState(owner),
  _levelNames{},
  _currentLevel{0},
  _level{},
  _controlScheme{nullptr},
  _marioLives{0},
  _score{0},
  _isCheating{true}
{
  assert(owner != nullptr);
}

bool PlayState::onInit()
{
  assert(_controlScheme == nullptr);
  assert(_currentLevel == 0);

  if(!loadDKConfig())
    return false;

  if(!_level.load(_levelNames[_currentLevel]))
    return false;

  _level.onInit(_controlScheme);

  return true;
}

void PlayState::onUpdate(double now, float dt)
{
  if(_isCheating)
    onCheatInput();

  _level.onUpdate(now, dt);



  //
  // note: this MUST be done last since it potentially replaces the level with a different one.
  //
  if(_level.isOver()){
    Level::Ending ending = _level.getEnding();
    if(ending == Level::ENDING_LOSS)
      handleLevelLoss();
    else if(ending == Level::ENDING_WIN)
      handleLevelWin();
  }
}

void PlayState::onDraw(double now, float dt, int screenid)
{
  pxr::gfx::clearScreenShade(1, screenid);
  _level.onDraw(screenid);
}

void PlayState::onReset()
{
}

void PlayState::onCheatInput()
{
  if(pxr::input::isKeyPressed(nextLevelCheatKey))
    nextLevel(true);

  else if(pxr::input::isKeyPressed(prevLevelCheatKey))
    prevLevel(true);
}

bool PlayState::nextLevel(bool loop)
{
  ++_currentLevel;
  if(_currentLevel >= _levelNames.size()){
    if(loop)
      _currentLevel = 0;
    else
      return false;
  }

  _level.unload();

  if(!_level.load(_levelNames[_currentLevel]))
    std::exit(EXIT_FAILURE);

  _level.onInit(_controlScheme);

  return true;
}

bool PlayState::prevLevel(bool loop)
{
  --_currentLevel;
  if(_currentLevel < 0){
    if(loop)
      _currentLevel = _levelNames.size() - 1;
    else
      return false;
  }

  _level.unload();

  if(!_level.load(_levelNames[_currentLevel]))
    std::exit(EXIT_FAILURE);

  _level.onInit(_controlScheme);

  return true;
}

void PlayState::handleLevelWin()
{
  if(!nextLevel(false)){
    // TODO switch back to menu state or a game complete state or something
    std::cout << "won level" << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

void PlayState::handleLevelLoss()
{
  --_marioLives;

  std::cout << "lost level" << std::endl;
  std::cout << "mario lives = " << _marioLives << std::endl;

  if(_marioLives <= 0){
    // TODO switch back to menu state or game over state or something.
    std::exit(EXIT_FAILURE);
  }
  _level.reset();
}

bool PlayState::loadDKConfig()
{
  assert(_levelNames.size() == 0);
  assert(_controlScheme == nullptr);

  std::string xmlpath {};
  xmlpath += RESOURCE_PATH_DKCONFIG;
  xmlpath += RESOURCE_NAME_DKCONFIG;
  xmlpath += pxr::io::XML_FILE_EXTENSION;
  
  pxr::log::log(pxr::log::INFO, msg_load_start, xmlpath);

  auto onerror = [](){
    pxr::log::log(pxr::log::ERROR, msg_load_abort);
    return false;
  };

  XMLDocument doc {};
  if(!pxr::io::parseXmlDocument(&doc, xmlpath))
    return onerror();

  XMLElement* xmldkconfig {nullptr};
  XMLElement* xmlcontrols {nullptr};
  XMLElement* xmlmario {nullptr};
  XMLElement* xmllevels {nullptr};
  XMLElement* xmllevel {nullptr};

  if(!pxr::io::extractChildElement(&doc, &xmldkconfig, "dkconfig"))
    return onerror();

  if(!pxr::io::extractChildElement(xmldkconfig, &xmlcontrols, "controls"))
    return onerror();

  _controlScheme = std::make_shared<ControlScheme>();

  auto loadKey = [&xmlcontrols](const char* attribute, pxr::input::KeyCode& keyCode) -> bool {
    const char* keyString {nullptr};
    if(!pxr::io::extractStringAttribute(xmlcontrols, attribute, &keyString)) 
      return false;
    keyCode = pxr::input::keyStringToKeyCode(keyString);
    if(keyCode == pxr::input::KEY_COUNT){
     pxr::log::log(pxr::log::ERROR, msg_invalid_key, keyString);
     return false;
    }
    return true;
  };

  if(!loadKey("runLeft", _controlScheme->_runLeftKey)) return onerror();
  if(!loadKey("runRight", _controlScheme->_runRightKey)) return onerror();
  if(!loadKey("jump", _controlScheme->_jumpKey)) return onerror();
  if(!loadKey("climbUp", _controlScheme->_climbUpKey)) return onerror();
  if(!loadKey("climbDown", _controlScheme->_climbDownKey)) return onerror();

  if(!pxr::io::extractChildElement(xmldkconfig, &xmlmario, "mario"))
    return onerror();

  if(!pxr::io::extractIntAttribute(xmlmario, "lives", &_marioLives)) return onerror();
  _marioLives = std::clamp(_marioLives, 0, std::numeric_limits<int>::max());

  if(!pxr::io::extractChildElement(xmldkconfig, &xmllevels, "levels"))
    return onerror();

  if(!pxr::io::extractChildElement(xmllevels, &xmllevel, "level"))
    return onerror();

  do {
    const char* levelName {nullptr};
    if(!pxr::io::extractStringAttribute(xmllevel, "name", &levelName)) return onerror(); 
    _levelNames.push_back(levelName);

    xmllevel = xmllevel->NextSiblingElement("level");
  }
  while(xmllevel != 0);
  
  pxr::log::log(pxr::log::INFO, msg_load_success, xmlpath);

  return true;
}
