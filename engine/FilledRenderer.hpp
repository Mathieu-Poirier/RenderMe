#pragma once
#include "DataTypes.hpp"
#include "CameraMath.hpp"
#include "CameraSettings.hpp"

// Draw a single triangle using barycentric fill
inline void DrawFilledTriangle(Int2_t p0, Int2_t p1, Int2_t p2,
    char (&fb)[CameraSettings::screen_height][CameraSettings::screen_width],
    char ch = '#') {
    
    // Bounding box
    int minX = std::min({p0.x, p1.x, p2.x});
    int maxX = std::max({p0.x, p1.x, p2.x});
    int minY = std::min({p0.y, p1.y, p2.y});
    int maxY = std::max({p0.y, p1.y, p2.y});

    minX = std::clamp(minX, 0, CameraSettings::screen_width - 1);
    maxX = std::clamp(maxX, 0, CameraSettings::screen_width - 1);
    minY = std::clamp(minY, 0, CameraSettings::screen_height - 1);
    maxY = std::clamp(maxY, 0, CameraSettings::screen_height - 1);

    // Edge function
    auto edge = [](const Int2_t& a, const Int2_t& b, const Int2_t& c) -> int {
        return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
    };

    int area = edge(p0, p1, p2);
    if (area == 0) return; // Degenerate triangle

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            Int2_t p = {x, y};
            int w0 = edge(p1, p2, p);
            int w1 = edge(p2, p0, p);
            int w2 = edge(p0, p1, p);

            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                fb[y][x] = ch;
            }
        }
    }
}

// Public triangle mesh renderer using filled triangles
inline void RenderMeshFilled(const Vec3Buffer& verts,
                             const TriangleBuffer& tris,
                             const Vec3_t& eye,
                             const Vec3_t& target,
                             char (&fb)[CameraSettings::screen_height][CameraSettings::screen_width],
                             char fillChar = '#') {
    
    auto view = LookAt(eye, target, {0, 1, 0});
    const double focal = CameraSettings::FovToFocalLength(CameraSettings::camera_fov);

    for (const auto& tri : tris.indices) {
        Vec3Buffer cam;
        WorldToCamera(verts, tri[0], view, eye, cam);
        WorldToCamera(verts, tri[1], view, eye, cam);
        WorldToCamera(verts, tri[2], view, eye, cam);
        if (cam.z[0] <= 0 || cam.z[1] <= 0 || cam.z[2] <= 0) continue;

        Vec2Buffer proj;
        ProjectToScreen(cam, 0, focal, CameraSettings::aspect_ratio, proj);
        ProjectToScreen(cam, 1, focal, CameraSettings::aspect_ratio, proj);
        ProjectToScreen(cam, 2, focal, CameraSettings::aspect_ratio, proj);

        Int2_t p0 = MapToScreen(proj, 0, CameraSettings::screen_width, CameraSettings::screen_height);
        Int2_t p1 = MapToScreen(proj, 1, CameraSettings::screen_width, CameraSettings::screen_height);
        Int2_t p2 = MapToScreen(proj, 2, CameraSettings::screen_width, CameraSettings::screen_height);

        DrawFilledTriangle(p0, p1, p2, fb, fillChar);
    }
}
