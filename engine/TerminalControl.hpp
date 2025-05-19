#pragma once
#include "CameraSettings.hpp"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <csignal>
#include <iostream>
#include <unordered_map>
#include <string>

namespace Terminal {

// ─────────────────────────────────────────────
// Cached terminal size info
// ─────────────────────────────────────────────

inline int term_width = 0;
inline int term_height = 0;
inline int last_width = -1;
inline int last_height = -1;
inline bool too_small = false;

inline void UpdateTerminalSize() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    term_width = w.ws_col;
    term_height = w.ws_row;

    too_small = (term_width < CameraSettings::screen_width ||
                 term_height < CameraSettings::screen_height);
}

inline bool DidTerminalResize() {
    bool changed = (term_width != last_width || term_height != last_height);
    last_width = term_width;
    last_height = term_height;
    return changed;
}

// ─────────────────────────────────────────────
// Terminal state control
// ─────────────────────────────────────────────

inline struct termios original_termios;

inline void InitTerminal() {
    tcgetattr(STDIN_FILENO, &original_termios);
    struct termios raw = original_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    std::cout << "\033[?25l\033[2J\033[H" << std::flush;
}

inline void RestoreTerminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
    std::cout << "\033[?25h\n" << std::flush;
}

// ─────────────────────────────────────────────
// Non-blocking raw input
// ─────────────────────────────────────────────

inline int ReadKeyRaw() {
    int ch = -1;
    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    return ch;
}

// ─────────────────────────────────────────────
// Key state tracking
// ─────────────────────────────────────────────

inline std::unordered_map<int, bool> key_state;
inline std::unordered_map<int, bool> key_prev;

inline void PollKeys() {
    key_prev = key_state;
    key_state.clear();
    int ch;
    while ((ch = ReadKeyRaw()) != -1) {
        key_state[ch] = true;
    }
}

inline bool IsKeyPressed(int keycode) {
    return key_state.count(keycode) > 0;
}

inline bool WasKeyJustPressed(int keycode) {
    return key_state[keycode] && !key_prev[keycode];
}

inline bool WasKeyJustReleased(int keycode) {
    return !key_state[keycode] && key_prev[keycode];
}

// ─────────────────────────────────────────────
// Debug utilities
// ─────────────────────────────────────────────

inline std::string PrintableChar(int ch) {
    if (ch >= 32 && ch < 127) return std::string(1, static_cast<char>(ch));
    if (ch == 27) return "ESC";
    return "0x" + std::to_string(ch);
}

inline void DumpKeyState() {
    std::cout << "[Keys Pressed]: ";
    for (const auto& [code, down] : key_state)
        if (down) std::cout << PrintableChar(code) << " ";
    std::cout << '\n';
}

} // namespace Terminal
