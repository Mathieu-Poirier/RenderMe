#include "CameraSettings.hpp"
#include "FrameBuffer.hpp"
#include "KeyMap.hpp"
#include "MeshBuilder.hpp"
#include "RenderMeshComposite.hpp"
#include "TerminalControl.hpp"
#include "DebugUI.hpp"

#include <chrono>
#include <cmath>
#include <thread>
#include <csignal>
#include <cstdlib>

// Ensure terminal is restored on exit or signal
void OnExit() {
  Terminal::RestoreTerminal();
}

void SignalHandler(int) {
  std::exit(0);
}

int main() {
  using Clock = std::chrono::steady_clock;
  auto last = Clock::now();

  std::atexit(OnExit);
  std::signal(SIGINT, SignalHandler);
  std::signal(SIGTERM, SignalHandler);

  Vec3_t target = {0, 0, 0};

  char front[CameraSettings::screen_height][CameraSettings::screen_width];
  char back[CameraSettings::screen_height][CameraSettings::screen_width];
  double zbuf[CameraSettings::screen_height][CameraSettings::screen_width];

  Vec3_t a = {-1, 0, -1};
  Vec3_t b = {1, 0, -1};
  Vec3_t c = {0, 0, 1};
  Vec3_t apex = {0, 2, 0};
  Vec3Buffer verts;
  TriangleBuffer tris;
  BuildPyramidMesh(a, b, c, apex, verts, tris);

  double angle = 0.0;
  bool paused = false;

  Terminal::InitTerminal();

  while (true) {
    auto now = Clock::now();
    double dt = std::chrono::duration<double>(now - last).count();
    last = now;

    Terminal::PollKeys();
    Terminal::UpdateTerminalSize();
    bool resized = Terminal::DidTerminalResize();

    if (Terminal::too_small) {
      std::cout << "\033[2J\033[H";
      std::cout << "⛔ Terminal too small. Resize to at least "
                << CameraSettings::screen_width << "x"
                << CameraSettings::screen_height << ".\n";
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      continue;
    }

    if (resized) {
      FrameIO::ClearFramebuffer(front);
      FrameIO::ClearFramebuffer(back);
      FrameIO::ClearZBuffer(zbuf);
      std::cout << "\033[2J\033[H";
      continue;
    }

    if (Terminal::WasKeyJustPressed(Key::Q)) break;
    if (Terminal::WasKeyJustPressed(Key::SPACE)) paused = !paused;

#if DEBUG_ENABLED
    if (Terminal::WasKeyJustPressed(Key::D))
      DebugUI::Toggle();
#endif

    if (!paused)
      angle += dt * 0.75;

    Vec3_t eye = {std::sin(angle) * 6.0, 3.0, std::cos(angle) * 6.0};

    FrameIO::ClearFramebuffer(back);
    FrameIO::ClearZBuffer(zbuf);

    RenderMeshComposite(verts, tris, eye, target, back, zbuf, '.', '*');

#if DEBUG_ENABLED
    DebugUI::Draw(back, eye, target, 1.0 / dt);
#endif

    if (!FrameIO::CompareBuffers(front, back)) {
      FrameIO::RenderChangedLines(back, front);
      FrameIO::CopyBuffer(front, back);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }

  return 0;
}
