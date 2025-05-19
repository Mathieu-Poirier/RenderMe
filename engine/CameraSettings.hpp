#pragma once

#include <cmath>

namespace CameraSettings {
constexpr int screen_width = 80;
constexpr int screen_height = 24;
constexpr double PI = 3.14159265358979323846;
constexpr double aspect_ratio =
    static_cast<double>(screen_width) / screen_height;
constexpr double camera_fov = 40.0;
constexpr double near_plane = 0.1;
constexpr double far_plane = 100.0;
constexpr double pixel_aspect = 2.0;

constexpr double half_width = screen_width * 0.5;
constexpr double half_height = screen_height * 0.5;


inline constexpr double FovToFocalLength(const double &fov) {
  return 1.0 / std::tan((fov / 2.0) * (PI / 180.0));
}

// Runtime constant for now (must be declared `inline` to avoid linker issues in
// headers)
inline const double &focal_length = FovToFocalLength(camera_fov);
} // namespace CameraSettings
