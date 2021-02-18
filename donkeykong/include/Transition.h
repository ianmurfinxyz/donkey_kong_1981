#ifndef PIXIRETRO_GAME_TRANSITION_H_
#define PIXIRETRO_GAME_TRANSITION_H_

#include <vector>

#include "pixiretro/pxr_vec.h"

//
// Represents a transition along a path through a 2D cartesian coordinate space.
//
//    y    [ Some 2D cartesian space ]
//    ^
//    |                                       The numbers form an ordered set
//    |                                       of points in said space which
//    |       1 . . . . . . . 2               together define a path.
//    |      .                  .     
//    |     .                    .            The path may or may not close.
//    |    .                      3 . . 4
//    |   0
//    |
//    o------------------------------------> x
//
// The position of the path will begin at the first point in the position points set and
// move along the path to the last point. Upon path completion the transition resets to move
// again from the first to last points, thus looping endlessly. The position thus jumps from 
// first to last instantaneously upon each full path completion. To avoid such jumping you
// must explicitly close the path by terminating the position points set with the first point,
// thus duplicating the first point.
//
// The speed of movement along the path is controlled by the set of speed points. This set 
// defines a set of speed values along a timeline.
//
//  speed
//    ^
//    |                                        vn is a value of speed at some
//    |     v1                                 point in time.
//    |   .    .                    v3      
//    | .         .             .  
//    v0              .      .
//    |                 v2
//    |  
//    |
//    o----------------------------------------> time
//
// The current value of speed moves along this speed path, moving linearly between the points.
// Thus this speed path shows the shape of acceleration over time.
//
// Like the position path the speed path loops endlessly. However unlike the position path, the 
// speed path is implicitly closed such that the speed graph above can be considered a subset 
// of a larger speed graph shown below which extends inifinitely along the time axis.
//
//  speed
//    ^
//    |                                        
//    |     v1                                  v1                              
//    |   .    .                    v3        .    .                    v3         
//    | .         .             .      .    .         .             .       .        ... 
//    v0              .      .            v0              .      .             v0
//    |                 v2                                   v2            
//    |  
//    |
//    o-------------------------------------------------------------------------------> time
//
// The subset graph is effectively a single cycle in an infinite speed wave. An important 
// difference between the speed and position paths is that there cannot be any instantaneous 
// changes in speed.
//
// The position and speed paths are completely decoupled. Each cycle of the speed wave may 
// complete in an arbitrary number of loops of the position path.
//
// All transition instances required at least 1 element in both point sets. A single position 
// point creates a static transition that does nothing. A single speed point creates
// constant movement along the position path.
//
// Note that although the class is written in terms of position and velocity, it can be used
// to move through arbitrary 2D spaces. For example you could define a velocity space in 
// which each point in the 2D space is a velocity. The speed wave then defines the speed of 
// movement through the velocity space, thus each point on the wave is a value of acceleration
// in time, and the shape of the wave shows the shape of jerk.
//
class Transition
{
public:

  struct SpeedPoint
  {
    float _value;     // value of speed at this point.
    float _duration;  // duration of transition from this point to the next.
  };

public:

  Transition(std::vector<pxr::Vector2f> positionPoints, std::vector<SpeedPoint> speedPoints);

  ~Transition() = default;

  Transition(const Transition&&) = default;
  Transition& operator=(const Transition&) = default;

  Transition(Transition&&) = default;
  Transition& operator=(Transition&&) = default;

  //
  // Call periodically every update tick to integrate the transition along the path.
  //
  pxr::Vector2f onUpdate(float dt);

  //
  // Resets the transition to start over.
  //
  void reset();

  //
  // Current position along the position path.
  //
  inline pxr::Vector2f getPosition() const;

  //
  // Current speed of movement along the position path.
  //
  inline float getSpeed() const;

private:

  std::vector<pxr::Vector2f> _positionPoints;
  std::vector<SpeedPoint> _speedPoints;
  pxr::Vector2f _position; 
  pxr::Vector2f _direction;
  float _speed;
  float _lerpClock;
  int _fromPosition;
  int _toPosition;
  int _fromSpeed;
  int _toSpeed;
  bool _isMoving;
  bool _isAccelerating;
};

#endif
