#pragma once
#include "CameraSettings.hpp"
#include "DataTypes.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>

using std::cout;

Vec3_t VecAdd(const Vec3_t &a, const Vec3_t &b) {
  return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3_t VecSub(const Vec3_t &a, const Vec3_t &b) {
  return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec3_t VecScale(const Vec3_t &v, double s) {
  return {v.x * s, v.y * s, v.z * s};
}

double VecDot(const Vec3_t &a, const Vec3_t &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3_t VecCross(const Vec3_t &a, const Vec3_t &b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

double VecLength(const Vec3_t &v) {
  return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

double VecDistance(const Vec3_t &a, const Vec3_t &b) {
  return VecLength(VecSub(a, b));
}

Vec3_t VecLerp(const Vec3_t &a, const Vec3_t &b, double t) {
  return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t};
}

Vec3_t VecProject(const Vec3_t &a, const Vec3_t &b) {
  double scale = VecDot(a, b) / VecDot(b, b);
  return VecScale(b, scale);
}

void VecPrint(const Vec3_t &v) {
  std::cout << "Vec3(" << std::fixed << std::setprecision(6) << v.x << ", "
            << v.y << ", " << v.z << ")\n";
}

Vec3_t VecHadamard(const Vec3_t &a, const Vec3_t &b) {
  return {a.x * b.x, a.y * b.y, a.z * b.z};
}

Vec3_t VecAbs(const Vec3_t &v) { return {fabs(v.x), fabs(v.y), fabs(v.z)}; }

Vec3_t VecNegate(const Vec3_t &v) { return {-v.x, -v.y, -v.z}; }

double VecAngle(const Vec3_t &a, const Vec3_t &b) {
  double dot = VecDot(a, b);
  double lenA = VecLength(a);
  double lenB = VecLength(b);
  return acos(dot / (lenA * lenB));
}

Vec3_t VecNormalize(const Vec3_t &v) {
  double len = VecLength(v);
  return len == 0.0 ? Vec3_t{0, 0, 0} : VecScale(v, 1.0 / len);
}

bool VecEquals(const Vec3_t &a, const Vec3_t &b, double epsilon = 1e-6) {
  return fabs(a.x - b.x) < epsilon && fabs(a.y - b.y) < epsilon &&
         fabs(a.z - b.z) < epsilon;
}

CameraView_t LookAt(const Vec3_t &eye, const Vec3_t &target, const Vec3_t &up) {
  // Step 1: Compute initial forward direction
  Vec3_t forward =
      VecNormalize(VecSub(target, eye)); // camera looks from eye to target

  // Step 2: Handle edge case — forward aligned or anti-aligned with up
  if (VecEquals(forward, up) || VecEquals(forward, VecNegate(up))) {
    // Try adjusting up slightly to break the alignment
    Vec3_t tempUp = {up.y, up.x, up.z}; // simple component shuffle
    forward = VecNormalize(VecCross(tempUp, forward));

    // Recheck after adjustment
    if (VecEquals(forward, up) || VecEquals(forward, VecNegate(up))) {
      tempUp = {up.x, up.z, up.y}; // alternate shuffle
      forward = VecNormalize(VecCross(tempUp, forward));
    }
  }

  // Step 3: Compute right vector from forward and up
  Vec3_t right = VecNormalize(VecCross(forward, up));

  // Step 4: Compute true up vector from right and forward
  Vec3_t cameraUp = VecNormalize(VecCross(right, forward));

  // Step 5: Handle edge case — cameraUp aligned with negative world up
  if (VecEquals(cameraUp, VecNegate(up))) {
    // Perturb right vector to fix orientation
    Vec3_t tempRight = {right.y, right.x, right.z}; // minor shuffle
    right = VecNormalize(VecCross(forward, tempRight));
    cameraUp = VecNormalize(VecCross(right, forward));
  }

  // Step 6: Compute camera-space offset (translation)
  Vec3_t offset = {-VecDot(right, eye), -VecDot(cameraUp, eye),
                   -VecDot(forward, eye)};

  // Step 7: Return the view basis and offset
  return {right, cameraUp, forward, offset};
}

Vec3_t WorldToCamera(const Vec3_t &worldPos, const CameraView_t &view,
                     const Vec3_t &eye) {
  Vec3_t relativePos = VecSub(worldPos, eye); // real camera-relative position
  return {VecDot(relativePos, view.right), VecDot(relativePos, view.up),
          VecDot(relativePos, view.forward)};
}

constexpr double FovToFocalLength(const double fov) {
  return 1.0 / tan((fov / 2.0) * (M_PI / 180.0));
}

Vec2_t ProjectToScreen(const Vec3_t &camPoint, const double focal_length,
                       const double aspect_ratio) {
  assert(camPoint.z > 0.0 && "Point is behind the camera!");

  double inv_z = 1.0 / camPoint.z;
  double fx = (focal_length / aspect_ratio) * CameraSettings::pixel_aspect;

  return {camPoint.x * inv_z * fx, camPoint.y * inv_z * focal_length};
}

Int2_t MapToScreen(const Vec2_t &projected, int screen_width,
                   const int screen_height) {

  constexpr double half = 0.5;
  const double inv_width = static_cast<double>(screen_width);
  const double inv_height = static_cast<double>(screen_height);

  double x_ndc = (projected.x + 1.0) * half;
  double y_ndc =
      (1.0 - ((projected.y + 1.0) * half)); // Flip Y for terminal layout

  int x = static_cast<int>(x_ndc * inv_width);
  int y = static_cast<int>(y_ndc * inv_height);

  x = std::clamp(x, 0, screen_width - 1);
  y = std::clamp(y, 0, screen_height - 1);

  return {x, y};
}
