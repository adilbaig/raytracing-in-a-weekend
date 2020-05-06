#ifndef RAY_H
#define RAY_H

#include "color.hpp"

class ray
{
public:
  ray() {}
  ray(const point3 &origin, const vec3 &direction) : orig(origin), dir(direction) {}

  point3 origin() const { return orig; }
  vec3 direction() const { return dir; }

  point3 at(double t) const
  {
    return orig + t * dir; // P(t)=A+tb
  }

  point3 orig;
  vec3 dir;
};

#endif