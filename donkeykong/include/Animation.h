#ifndef _PIXIRETRO_GAME_ANIMATION_H_
#define _PIXIRETRO_GAME_ANIMATION_H_

#include <memory>
#include "pixiretro/pxr_gfx.h"
#include "pixiretro/pxr_vec.h"

class AnimationFactory;
struct AnimationDefinition;

class Animation
{
  friend class AnimationFactory;

public:

  //
  // The mode controls how the animation chooses which frame to show next. In mode:
  //
  //  - STATIC only a single frame is shown (the first frame).
  //  - FORWARD frames are shown one after the other in the order they are defined.
  //  - BACKWARD frames are shown in the reverse order in which they are defined.
  //  - RANDOM frame selection is random; any frame may shown after any other.
  //
  enum class Mode { STATIC, FORWARD, BACKWARD, RANDOM };

  // 
  // Construct a default, but invalid animation. Such an animation must be assigned
  // a valid animation returned from the animation factory.
  //
  // The point of this default is to make it easier to manage memory for animations external 
  // of the animation factory.
  //
  Animation();

  ~Animation() = default;

  Animation(const Animation& other) = default;
  Animation& operator=(const Animation& other) = default;

  Animation(Animation&& other) = default;
  Animation& operator=(Animation&& other) = default;

  //
  // Call periodically during the update tick.
  //
  void onUpdate(float dt);

  //
  // Draws the currently active frame (a sprite) to a screen at a specified position. The
  // position is taken as the position of the sprite origin.
  //
  void onDraw(pxr::Vector2i position, int screenid);

  //
  // Resets the animation to start from frame 0.
  //
  void reset();

  bool isMirroringX() const {return _mirrorX;}
  bool isMirroringY() const {return _mirrorY;} 
  void setMirrorX(bool mirror);
  void setMirrorY(bool mirror);

  float getPeriod() const {return _def->_period;}
  float getFrequency() const {return _def->_frequency;}
  int getFrameCount() const {return _def->_frames.size();}
  int getFrameNo() const {return _frameNo;}

  pxr::gfx::ResourceKey_t getSpritesheetKey() const {return _def->_spritesheetKey;}
  pxr::gfx::SpriteId_t getSpriteId() const {return _def->_frames[_frameNo];}

public:

  //
  // A definition defines an animation type and encapsulates all type specific data shared by
  // all instances of a given type. These definitions are loaded from the animations definitions 
  // file by the animation factory.
  //
  struct Definition
  {

    //
    // Constructor to establish the following invariants:
    //
    //   - name != "" (empty string)
    //   - frequency > 0
    //
    Definition(std::string                       name,
               Mode                              mode,
               pxr::gfx::ResourceKey_t           spritesheetKey,
               std::vector<pxr::gfx::SpriteId_t> frames,
               float                             frequency,
               bool                              baseMirrorX,
               bool                              baseMirrorY 
    );
               
    std::string _name;
    Mode _mode;
    pxr::gfx::ResourceKey_t _spritesheetKey;
    std::vector<pxr::gfx::SpriteId_t> _frames;
    float _frequency;
    float _period;

    //
    // Should the sprites be mirrored when drawing.
    //
    bool _baseMirrorX;
    bool _baseMirrorY;
  };

private:

  //
  // Accessible only by the factory.
  //
  Animation(std::shared_ptr<const Definition> def);

private:

  std::shared_ptr<const Definition> _def;
  int _frameNo;
  float _clock;

  //
  // If mirror == true, then baseMirror is inverted.
  // if mirror == false, then baseMirror is not inverted, i.e. the base mirror is used.
  //
  bool _mirrorX;
  bool _mirrorY;
};

#endif
