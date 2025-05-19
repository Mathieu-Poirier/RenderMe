#pragma once
#include "CameraSettings.hpp"
#include <iostream>
#include <cstring>

using Frame = char[CameraSettings::screen_height][CameraSettings::screen_width];

inline void ClearFramebuffer(Frame& fb, char fill = ' ') {
    for (int y = 0; y < CameraSettings::screen_height; ++y)
        for (int x = 0; x < CameraSettings::screen_width; ++x)
            fb[y][x] = fill;
}

inline bool CompareBuffers(const Frame& a, const Frame& b) {
    return std::memcmp(a, b, sizeof(Frame)) == 0;
}

inline void CopyBuffer(Frame& dst, const Frame& src) {
    std::memcpy(dst, src, sizeof(Frame));
}

inline void RenderFramebuffer(const Frame& fb) {
    std::cout << "\033[H"; // reset cursor to top-left
    for (int y = 0; y < CameraSettings::screen_height; ++y) {
        for (int x = 0; x < CameraSettings::screen_width; ++x)
            std::cout << fb[y][x];
        std::cout << '\n';
    }
    std::cout << std::flush;
}
