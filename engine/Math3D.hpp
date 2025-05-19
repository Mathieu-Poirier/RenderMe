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

constexpr double FovToFocalLength(const double fov) {
    return 1.0 / std::tan((fov / 2.0) * (M_PI / 180.0));
}
