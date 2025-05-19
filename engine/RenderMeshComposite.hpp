#pragma once
#include "FilledRenderer.hpp"
#include "WireframeRenderer.hpp"

// Renders filled triangles, then outlines over top
inline void RenderMeshComposite(
    const Vec3Buffer& verts,
    const TriangleBuffer& tris,
    const Vec3_t& eye,
    const Vec3_t& target,
    char (&fb)[CameraSettings::screen_height][CameraSettings::screen_width],
    double (&zbuf)[CameraSettings::screen_height][CameraSettings::screen_width],
    char fillChar = '#',
    char lineChar = '*'
) {
    // Fill first
    RenderMeshFilled(verts, tris, eye, target, fb, zbuf, fillChar);

    // Outline last
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
        DrawLine(p0, p1, fb, lineChar);
    }
}
