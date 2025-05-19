#include "CameraSettings.hpp"
#include "DataTypes.hpp"
#include "FrameBuffer.hpp"
#include "Math3D.hpp"

#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

using std::cout, std::endl;
using namespace std::chrono_literals;

int main() {
  Vec3_t target = {0.0, 0.0, 0.0};
  Vec3_t up = {0.0, 1.0, 0.0};

  // Hide cursor
  std::cout << "\033[?25l";

  // Cube vertices
  Vec3_t points[] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
                     {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}};

  double t = 0.0;
  while (true) {
    // Circular camera motion around cube
    double radius = 5.0;
    double speed = 0.5;
    Vec3_t eye = {radius * std::cos(t) * 2, 0.05 * t * t + std::sin(t),
                  radius * std::sin(t) * 2};

    CameraView_t view = LookAt(eye, target, up);
    char framebuffer[CameraSettings::screen_height]
                    [CameraSettings::screen_width];
    ClearFramebuffer(framebuffer);

    for (Vec3_t &world : points) {
      Vec3_t cam = WorldToCamera(world, view, eye);
      if (cam.z <= CameraSettings::near_plane ||
          cam.z >= CameraSettings::far_plane)
        continue;
      assert(cam.z < CameraSettings::far_plane &&
             "Object beyond far clipping plane!");

      Vec2_t projected = ProjectToScreen(cam, CameraSettings::focal_length,
                                         CameraSettings::aspect_ratio);

      double x_ndc = (projected.x + 1.0) * 0.5;
      double y_ndc = 1.0 - ((projected.y + 1.0) * 0.5);

      int x = static_cast<int>(x_ndc * CameraSettings::screen_width);
      int y = static_cast<int>(y_ndc * CameraSettings::screen_height);

      if (x >= 0 && x < CameraSettings::screen_width && y >= 0 &&
          y < CameraSettings::screen_height) {
        framebuffer[y][x] = '@';
      }
    }

    std::cout << "\033[H"; // Move cursor to top-left
    RenderFramebuffer(framebuffer);

    std::this_thread::sleep_for(5ms);
    t += speed * 0.04;
  }

  // Show cursor again (unreachable in current loop)
  std::cout << "\033[?25h";

  return 0;
}
