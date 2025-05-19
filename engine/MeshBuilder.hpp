#pragma once
#include "DataTypes.hpp"

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