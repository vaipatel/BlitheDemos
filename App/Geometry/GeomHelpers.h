#ifndef GEOMHELPERS_H
#define GEOMHELPERS_H

#include <glm/glm.hpp>
#include <vector>

namespace blithe
{
    struct Mesh;

    class GeomHelpers
    {
    public:
        static glm::vec3 CalcCentroid(const Mesh& _mesh);
        static Mesh CreateCuboid(glm::vec3 _sides, const std::vector<glm::vec4>& _colors);
    };
}

#endif //GEOMHELPERS_H
