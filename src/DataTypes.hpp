#pragma once

struct Vec3_t {
  double x, y, z;
};

struct Vec2_t {
  double x, y;
};

struct Int2_t {
  int x, y;
};

struct CameraView_t {
  Vec3_t right;   // x-axis
  Vec3_t up;      // y-axis
  Vec3_t forward; // z-axis (usually looking -Z)
  Vec3_t
      offset; // translation: (-dot(right,eye), -dot(up,eye), -dot(forward,eye))
};