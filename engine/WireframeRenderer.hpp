#pragma once
#include "DataTypes.hpp"
#include "CameraMath.hpp"
#include "CameraSettings.hpp"
#include <unordered_set>

// Extract unique edges from triangle mesh
inline std::unordered_set<Edge> ExtractEdges(const TriangleBuffer& tris) {
    std::unordered_set<Edge> edges;
    for (const auto& tri : tris.indices) {
        edges.insert(Edge(tri[0], tri[1]));
        edges.insert(Edge(tri[1], tri[2]));
        edges.insert(Edge(tri[2], tri[0]));
    }
    return edges;
}

// Bresenham-style line draw
inline void DrawLine(Int2_t a, Int2_t b,
    char (&fb)[CameraSettings::screen_height][CameraSettings::screen_width],
    char ch = '*') {
    int x0 = a.x, y0 = a.y, x1 = b.x, y1 = b.y;
    int dx = std::abs(x1 - x0), dy = -std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1, sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    while (true) {
        if (x0 >= 0 && x0 < CameraSettings::screen_width &&
            y0 >= 0 && y0 < CameraSettings::screen_height)
            fb[y0][x0] = ch;
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// Full render from mesh + camera
inline void RenderMeshOutline(const Vec3Buffer& verts,
                              const TriangleBuffer& tris,
                              const Vec3_t& eye,
                              const Vec3_t& target,
                              char (&fb)[CameraSettings::screen_height][CameraSettings::screen_width]) {
    auto edges = ExtractEdges(tris);
    auto view = LookAt(eye, target, CAMERA_UP);
    const double focal = CameraSettings::FovToFocalLength(CameraSettings::camera_fov);

    for (const auto& e : edges) {
        Vec3Buffer cam;
        WorldToCamera(verts, e.a, view, eye, cam);
        WorldToCamera(verts, e.b, view, eye, cam);
        if (cam.z[0] <= 0 || cam.z[1] <= 0) continue;

        Vec2Buffer proj;
        ProjectToScreen(cam, 0, focal, CameraSettings::aspect_ratio, proj);
        ProjectToScreen(cam, 1, focal, CameraSettings::aspect_ratio, proj);
        Int2_t p0 = MapToScreen(proj, 0, CameraSettings::screen_width, CameraSettings::screen_height);
        Int2_t p1 = MapToScreen(proj, 1, CameraSettings::screen_width, CameraSettings::screen_height);
        DrawLine(p0, p1, fb);
    }
}
