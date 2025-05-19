#pragma once
#include "CameraSettings.hpp"
#include "TerminalControl.hpp"
#include "KeyMap.hpp"
#include "DataTypes.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

#define DEBUG_ENABLED 1

namespace DebugUI {

// State
inline bool show_debug = false;
inline std::vector<std::string> debug_lines;

// Toggle display (called on debug key press)
inline void Toggle() {
    show_debug = !show_debug;
}

// Clear history/log
inline void Clear() {
    debug_lines.clear();
}

// Add manual message
inline void Log(const std::string& msg) {
    debug_lines.push_back(msg);
    if (debug_lines.size() > 6) // keep it tidy
        debug_lines.erase(debug_lines.begin());
}

// Format vector
inline std::string FormatVec3(const Vec3_t& v) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2)
        << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return oss.str();
}

// Update and draw debug info
inline void Draw(
    char (&fb)[CameraSettings::screen_height][CameraSettings::screen_width],
    const Vec3_t& camera_pos,
    const Vec3_t& target,
    double fps) {

    if (!show_debug) return;

    std::vector<std::string> lines;

    // Core status
    lines.push_back("[Debug Info]");
    lines.push_back(" FPS: " + std::to_string(static_cast<int>(fps)));
    lines.push_back(" Eye: " + FormatVec3(camera_pos));
    lines.push_back(" At : " + FormatVec3(target));

    // Input state
    std::ostringstream keys;
    keys << " Keys: ";
    for (const auto& [code, down] : Terminal::key_state) {
        if (down) keys << Terminal::PrintableChar(code) << " ";
    }
    lines.push_back(keys.str());

    // Manual logs
    for (const auto& line : debug_lines)
        lines.push_back("> " + line);

    // Write into framebuffer at top-left
    for (size_t i = 0; i < lines.size(); ++i) {
        if (i >= static_cast<size_t>(CameraSettings::screen_height)) break;
        const std::string& line = lines[i];
        for (size_t x = 0; x < line.size() && x < CameraSettings::screen_width; ++x)
            fb[i][x] = line[x];
    }
}

} // namespace DebugUI
