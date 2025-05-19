#pragma once
#include "CameraSettings.hpp"
#include "DataTypes.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>

using std::cout;

// === Atomic (AoS) versions for single Vec3_t math ===

inline Vec3_t VecAddAtomic(const Vec3_t& a, const Vec3_t& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline Vec3_t VecSubAtomic(const Vec3_t& a, const Vec3_t& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

inline Vec3_t VecScaleAtomic(const Vec3_t& v, double s) {
    return {v.x * s, v.y * s, v.z * s};
}

inline double VecDotAtomic(const Vec3_t& a, const Vec3_t& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3_t VecCrossAtomic(const Vec3_t& a, const Vec3_t& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

inline double VecLengthAtomic(const Vec3_t& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline Vec3_t VecNormalizeAtomic(const Vec3_t& v) {
    double len = VecLengthAtomic(v);
    return len == 0.0 ? Vec3_t{0, 0, 0} : VecScaleAtomic(v, 1.0 / len);
}

inline Vec3_t VecNegateAtomic(const Vec3_t& v) {
    return {-v.x, -v.y, -v.z};
}

inline bool VecEqualsAtomic(const Vec3_t& a, const Vec3_t& b, double epsilon = 1e-6) {
    return std::fabs(a.x - b.x) < epsilon &&
           std::fabs(a.y - b.y) < epsilon &&
           std::fabs(a.z - b.z) < epsilon;
}

// === SoA (Vec3Buffer) versions for indexed batch math ===

inline void VecAdd(const Vec3Buffer &a, size_t ai,
                   const Vec3Buffer &b, size_t bi,
                   Vec3Buffer &out) {
    out.push_back(
        a.x[ai] + b.x[bi],
        a.y[ai] + b.y[bi],
        a.z[ai] + b.z[bi]
    );
}

inline void VecSubAtomic(const Vec3Buffer &a, size_t ai,
                   const Vec3Buffer &b, size_t bi,
                   Vec3Buffer &out) {
    Vec3_t res = VecSubAtomic({a.x[ai], a.y[ai], a.z[ai]}, {b.x[bi], b.y[bi], b.z[bi]});
    out.push_back(res.x, res.y, res.z);
}

inline void VecScaleAtomic(const Vec3Buffer &v, size_t vi,
                     double s,
                     Vec3Buffer &out) {
    Vec3_t res = VecScaleAtomic({v.x[vi], v.y[vi], v.z[vi]}, s);
    out.push_back(res.x, res.y, res.z);
}

inline double VecDotAtomic(const Vec3Buffer &a, size_t ai,
                     const Vec3Buffer &b, size_t bi) {
    return VecDotAtomic({a.x[ai], a.y[ai], a.z[ai]}, {b.x[bi], b.y[bi], b.z[bi]});
}

inline void VecCrossAtomic(const Vec3Buffer &a, size_t ai,
                     const Vec3Buffer &b, size_t bi,
                     Vec3Buffer &out) {
    Vec3_t res = VecCrossAtomic({a.x[ai], a.y[ai], a.z[ai]}, {b.x[bi], b.y[bi], b.z[bi]});
    out.push_back(res.x, res.y, res.z);
}

inline double VecLengthAtomic(const Vec3Buffer &v, size_t vi) {
    return VecLengthAtomic({v.x[vi], v.y[vi], v.z[vi]});
}

inline double VecDistance(const Vec3Buffer &a, size_t ai,
                          const Vec3Buffer &b, size_t bi) {
    Vec3_t da = {a.x[ai] - b.x[bi], a.y[ai] - b.y[bi], a.z[ai] - b.z[bi]};
    return VecLengthAtomic(da);
}

inline void VecLerp(const Vec3Buffer &a, size_t ai,
                    const Vec3Buffer &b, size_t bi,
                    double t,
                    Vec3Buffer &out) {
    Vec3_t va = {a.x[ai], a.y[ai], a.z[ai]};
    Vec3_t vb = {b.x[bi], b.y[bi], b.z[bi]};
    Vec3_t res = {
        va.x + (vb.x - va.x) * t,
        va.y + (vb.y - va.y) * t,
        va.z + (vb.z - va.z) * t
    };
    out.push_back(res.x, res.y, res.z);
}

inline void VecNormalizeAtomic(const Vec3Buffer &v, size_t vi,
                         Vec3Buffer &out) {
    Vec3_t res = VecNormalizeAtomic({v.x[vi], v.y[vi], v.z[vi]});
    out.push_back(res.x, res.y, res.z);
}

inline void VecProject(const Vec3Buffer &a, size_t ai,
                       const Vec3Buffer &b, size_t bi,
                       Vec3Buffer &out) {
    Vec3_t va = {a.x[ai], a.y[ai], a.z[ai]};
    Vec3_t vb = {b.x[bi], b.y[bi], b.z[bi]};
    double scale = VecDotAtomic(va, vb) / VecDotAtomic(vb, vb);
    Vec3_t res = VecScaleAtomic(vb, scale);
    out.push_back(res.x, res.y, res.z);
}

inline void VecAbs(const Vec3Buffer &v, size_t vi,
                   Vec3Buffer &out) {
    out.push_back(
        std::fabs(v.x[vi]),
        std::fabs(v.y[vi]),
        std::fabs(v.z[vi])
    );
}

inline void VecNegateAtomic(const Vec3Buffer &v, size_t vi,
                      Vec3Buffer &out) {
    Vec3_t res = VecNegateAtomic({v.x[vi], v.y[vi], v.z[vi]});
    out.push_back(res.x, res.y, res.z);
}

inline bool VecEqualsAtomic(const Vec3Buffer &a, size_t ai,
                      const Vec3Buffer &b, size_t bi,
                      double epsilon = 1e-6) {
    return VecEqualsAtomic({a.x[ai], a.y[ai], a.z[ai]}, {b.x[bi], b.y[bi], b.z[bi]}, epsilon);
}

// --- Camera setup remains AoS for single camera ---
inline CameraView_t LookAt(const Vec3_t &eye,
                           const Vec3_t &target,
                           const Vec3_t &up) {
    Vec3_t forward = VecNormalizeAtomic(VecSubAtomic(target, eye));
    if (VecEqualsAtomic(forward, up) || VecEqualsAtomic(forward, VecNegateAtomic(up))) {
        Vec3_t tempUp = {up.y, up.x, up.z};
        forward = VecNormalizeAtomic(VecCrossAtomic(tempUp, forward));
        if (VecEqualsAtomic(forward, up) || VecEqualsAtomic(forward, VecNegateAtomic(up))) {
            tempUp = {up.x, up.z, up.y};
            forward = VecNormalizeAtomic(VecCrossAtomic(tempUp, forward));
        }
    }
    Vec3_t right    = VecNormalizeAtomic(VecCrossAtomic(forward, up));
    Vec3_t cameraUp = VecNormalizeAtomic(VecCrossAtomic(right, forward));
    if (VecEqualsAtomic(cameraUp, VecNegateAtomic(up))) {
        Vec3_t tempRight = {right.y, right.x, right.z};
        right    = VecNormalizeAtomic(VecCrossAtomic(forward, tempRight));
        cameraUp = VecNormalizeAtomic(VecCrossAtomic(right, forward));
    }
    Vec3_t offset = {
        -VecDotAtomic(right, eye),
        -VecDotAtomic(cameraUp, eye),
        -VecDotAtomic(forward, eye)
    };
    return { right, cameraUp, forward, offset };
}

inline void WorldToCamera(const Vec3Buffer &world,
                          size_t wi,
                          const CameraView_t &view,
                          const Vec3_t     &eye,
                          Vec3Buffer       &out) {
    double rx = world.x[wi] - eye.x;
    double ry = world.y[wi] - eye.y;
    double rz = world.z[wi] - eye.z;
    out.push_back(
        rx * view.right.x + ry * view.right.y + rz * view.right.z,
        rx * view.up.x    + ry * view.up.y    + rz * view.up.z,
        rx * view.forward.x + ry * view.forward.y + rz * view.forward.z
    );
}

inline void ProjectToScreen(const Vec3Buffer &cam,
                            size_t ci,
                            double focal_length,
                            double aspect_ratio,
                            Vec2Buffer       &out) {
    assert(cam.z[ci] > 0.0 && "Point is behind the camera!");
    double inv_z = 1.0 / cam.z[ci];
    double fx    = (focal_length / aspect_ratio) * CameraSettings::pixel_aspect;
    out.push_back(
        cam.x[ci] * inv_z * fx,
        cam.y[ci] * inv_z * focal_length
    );
}

inline Int2_t MapToScreen(const Vec2Buffer &proj,
                          size_t pi,
                          int screen_width,
                          int screen_height) {
    constexpr double half = 0.5;
    double x_ndc = (proj.x[pi] + 1.0) * half;
    double y_ndc = 1.0 - ((proj.y[pi] + 1.0) * half);
    int xi = static_cast<int>(std::round(x_ndc * screen_width));
    int yi = static_cast<int>(std::round(y_ndc * screen_height));
    xi = std::clamp(xi, 0, screen_width - 1);
    yi = std::clamp(yi, 0, screen_height - 1);
    return { xi, yi };
}

constexpr double FovToFocalLength(const double fov) {
    return 1.0 / std::tan((fov / 2.0) * (M_PI / 180.0));
}

void SampleLineBetweenPoints(const Vec3_t& a,
                             const Vec3_t& b,
                             int steps,
                             Vec3Buffer& out) {
    out.clear();
    out.x.reserve(steps + 1);
    out.y.reserve(steps + 1);
    out.z.reserve(steps + 1);

    for (int i = 0; i <= steps; ++i) {
        double t = static_cast<double>(i) / steps;
        Vec3_t p = VecAddAtomic(a, VecScaleAtomic(VecSubAtomic(b, a), t));
        out.push_back(p.x, p.y, p.z);
    }
}

void BuildTriangleMesh(const Vec3_t& a,
                       const Vec3_t& b,
                       const Vec3_t& c,
                       Vec3Buffer& verts,
                       TriangleBuffer& tris) {
    size_t base = verts.size(); // where we're inserting into the vertex buffer
    verts.push_back(a.x, a.y, a.z);
    verts.push_back(b.x, b.y, b.z);
    verts.push_back(c.x, c.y, c.z);
    tris.push_back(base, base + 1, base + 2);
}


void BuildPyramidWireframe(const Vec3_t& a,
                           const Vec3_t& b,
                           const Vec3_t& c,
                           const Vec3_t& apex,
                           int steps,
                           Vec3Buffer& out) {
    out.clear();
    Vec3Buffer temp;

    auto sample_edge = [&](const Vec3_t& p1, const Vec3_t& p2) {
        temp.clear();
        SampleLineBetweenPoints(p1, p2, steps, temp);
        for (size_t i = 0; i < temp.size(); ++i)
            out.push_back(temp.x[i], temp.y[i], temp.z[i]);
    };

    // Base triangle edges
    sample_edge(a, b);
    sample_edge(b, c);
    sample_edge(c, a);

    // Sides to apex
    sample_edge(a, apex);
    sample_edge(b, apex);
    sample_edge(c, apex);
}

void BuildPyramidMesh(const Vec3_t& a,
                      const Vec3_t& b,
                      const Vec3_t& c,
                      const Vec3_t& apex,
                      Vec3Buffer& verts_out,
                      TriangleBuffer& tris_out) {
    // Base
    BuildTriangleMesh(a, b, c, verts_out, tris_out);

    // Sides
    BuildTriangleMesh(a, b, apex, verts_out, tris_out);
    BuildTriangleMesh(b, c, apex, verts_out, tris_out);
    BuildTriangleMesh(c, a, apex, verts_out, tris_out);
}

