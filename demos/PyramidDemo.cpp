#include "DataTypes.hpp"
#include "Math3D.hpp"
#include <array>
#include <iostream>

int main() {
        Vec3_t a = {-1, 0, -1};
        Vec3_t b = {1, 0, -1};
        Vec3_t c = {0, 0, 1};
        Vec3_t apex = {0, 2, 0};

        Vec3Buffer verts;
        TriangleBuffer tris;

        BuildPyramidMesh(a, b, c, apex, verts, tris);

        std::cout << "Generated pyramid with " << verts.size()
                  << " vertices and " << tris.size() << " triangles.\n";

        return 0;
}
