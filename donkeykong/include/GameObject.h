
#include "pixiretro/collision.h"  // for AABB TODO implement this engine header

//
// functionality common to props and actors
//

class GameObject
{
public:

  virtual void onUpdate(double now, float dt);
  virtual void onIntegrate(float dt);
  virtual void onDraw(int screenid);

  inline Vector2f getPosition() const;
  inline Vector2f getVelocity() const;
  inline pxr::AABB getInteractionBox() const;
  inline bool isInteracting() const;

private:
  Vector2f _position;
  Vector2f _velocity;

  //
  // Game objects with overlapping interaction boxes are defined as
  // interacting.
  //
  pxr::AABB _interactionBox;

  //
  // Enable/disable interaction with other game objects.
  //
  bool _isInteracting;
};

