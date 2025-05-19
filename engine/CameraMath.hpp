#pragma once
#include "DataTypes.hpp"
#include "Math3D.hpp"
#include "CameraSettings.hpp"
#include <cmath>
#include <cassert>

constexpr Vec3_t CAMERA_UP = {0.0, 1.0, 0.0};


inline CameraView_t LookAt(const Vec3_t& eye,
                           const Vec3_t& target,
                           const Vec3_t& up) {
    Vec3_t forward = VecNormalizeAtomic(VecSubAtomic(target, eye));
    if (VecEqualsAtomic(forward, up) || VecEqualsAtomic(forward, VecNegateAtomic(up))) {
        Vec3_t tempUp = {up.y, up.x, up.z};
        forward = VecNormalizeAtomic(VecCrossAtomic(tempUp, forward));
        if (VecEqualsAtomic(forward, up) || VecEqualsAtomic(forward, VecNegateAtomic(up))) {
            tempUp = {up.x, up.z, up.y};
            forward = VecNormalizeAtomic(VecCrossAtomic(tempUp, forward));
        }
    }
    Vec3_t right = VecNormalizeAtomic(VecCrossAtomic(forward, up));
    Vec3_t cameraUp = VecNormalizeAtomic(VecCrossAtomic(right, forward));
    if (VecEqualsAtomic(cameraUp, VecNegateAtomic(up))) {
        Vec3_t tempRight = {right.y, right.x, right.z};
        right = VecNormalizeAtomic(VecCrossAtomic(forward, tempRight));
        cameraUp = VecNormalizeAtomic(VecCrossAtomic(right, forward));
    }
    Vec3_t offset = {
        -VecDotAtomic(right, eye),
        -VecDotAtomic(cameraUp, eye),
        -VecDotAtomic(forward, eye)
    };
    return { right, cameraUp, forward, offset };
}

inline void WorldToCamera(const Vec3Buffer& world,
                          size_t wi,
                          const CameraView_t& view,
                          const Vec3_t& eye,
                          Vec3Buffer& out) {
    double rx = world.x[wi] - eye.x;
    double ry = world.y[wi] - eye.y;
    double rz = world.z[wi] - eye.z;
    out.push_back(
        rx * view.right.x + ry * view.right.y + rz * view.right.z,
        rx * view.up.x    + ry * view.up.y    + rz * view.up.z,
        rx * view.forward.x + ry * view.forward.y + rz * view.forward.z
    );
}

inline void ProjectToScreen(const Vec3Buffer& cam,
                            size_t ci,
                            double focal_length,
                            double aspect_ratio,
                            Vec2Buffer& out) {
    assert(cam.z[ci] > 0.0 && "Point is behind the camera!");
    double inv_z = 1.0 / cam.z[ci];
    double fx = (focal_length / aspect_ratio) * CameraSettings::pixel_aspect;
    out.push_back(
        cam.x[ci] * inv_z * fx,
        cam.y[ci] * inv_z * focal_length
    );
}

inline Int2_t MapToScreen(const Vec2Buffer& proj,
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
