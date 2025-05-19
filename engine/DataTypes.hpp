#pragma once
#include <vector>
#include <cstddef> // for size_t
#include <array>


// --- Integer 2D struct (not SoA, typically used for screen positions) ---
struct Int2_t {
    int x, y;
};

// AoS is still appropriate here because this is a single logical object
struct Vec3_t {
    double x, y, z;
};

// Triangle index buffer (face layout)
struct TriangleBuffer {
    std::vector<std::array<size_t, 3>> indices;

    inline void clear() { indices.clear(); }

    inline void push_back(size_t i0, size_t i1, size_t i2) {
        indices.push_back({i0, i1, i2});
    }

    inline size_t size() const { return indices.size(); }
};


// --- Struct of Arrays for 3D vectors ---
struct Vec3Buffer {
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;

    Vec3Buffer() = default;

    Vec3Buffer(const Vec3_t* arr, size_t count) {
    x.reserve(count);
    y.reserve(count);
    z.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        x.push_back(arr[i].x);
        y.push_back(arr[i].y);
        z.push_back(arr[i].z);
    }
  }

    static Vec3Buffer FromArray(const Vec3_t* arr, size_t count) {
      return Vec3Buffer(arr, count);
    }

    inline void clear() {
        x.clear(); y.clear(); z.clear();
    }

    inline void push_back(double xi, double yi, double zi) {
        x.push_back(xi); y.push_back(yi); z.push_back(zi);
    }

    inline size_t size() const {
        return x.size(); // assume x, y, z are always the same length
    }
};

// --- Struct of Arrays for 2D vectors ---
struct Vec2Buffer {
    std::vector<double> x;
    std::vector<double> y;

    Vec2Buffer() = default;

    inline void clear() {
        x.clear(); y.clear();
    }

    inline void push_back(double xi, double yi) {
        x.push_back(xi); y.push_back(yi);
    }

    inline size_t size() const {
        return x.size();
    }
};

// --- Single camera frame, not SoA (represents 1 matrix transform) ---
struct CameraView_t {

    Vec3_t right;   // x-axis
    Vec3_t up;      // y-axis
    Vec3_t forward; // z-axis (usually looking -Z)
    Vec3_t offset;  // translation: (-dot(right,eye), -dot(up,eye), -dot(forward,eye))
};

// Edge struct (undirected)
struct Edge {
    size_t a, b;
    Edge(size_t i, size_t j) : a(std::min(i,j)), b(std::max(i,j)) {}
    bool operator==(const Edge& o) const { return a == o.a && b == o.b; }
};

namespace std {
    template <> struct hash<Edge> {
        size_t operator()(const Edge& e) const {
            return std::hash<size_t>()(e.a) ^ (std::hash<size_t>()(e.b) << 1);
        }
    };
}
