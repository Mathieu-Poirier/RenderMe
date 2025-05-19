#pragma once
#include "CameraSettings.hpp"
#include <iostream>

void ClearFramebuffer(
    char (&fb)[CameraSettings::screen_height][CameraSettings::screen_width]) {
  for (int y = 0; y < CameraSettings::screen_height; ++y) {
    for (int x = 0; x < CameraSettings::screen_width; ++x) {
      fb[y][x] = ' ';
    }
  }
}

void RenderFramebuffer(const char (
    &fb)[CameraSettings::screen_height][CameraSettings::screen_width]) {
  std::cout << "\033[?25l";

  for (int y = 0; y < CameraSettings::screen_height; ++y) {
    for (int x = 0; x < CameraSettings::screen_width; ++x) {
      std::cout << fb[y][x];
    }
    std::cout << '\n';
  }

  std::cout << std::flush;
}
