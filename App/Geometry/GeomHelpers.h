#ifndef GEOMHELPERS_H
#define GEOMHELPERS_H

#include <glm/glm.hpp>
#include <vector>
#include "AABB.h"

namespace blithe
{
    struct Mesh;

    class GeomHelpers
    {
    public:
        static glm::vec3 CalcCentroid(const Mesh& _mesh);
        static AABB CalcLocalAABB(const Mesh& _mesh);
        static AABB CalcWorldAABB(const Mesh& _mesh, const glm::mat4& _modelMat);
        static AABB TransformAABB(const AABB& _aabb, const glm::mat4& _mat);
        static Mesh CreateCuboid(glm::vec3 _sides, const std::vector<glm::vec4>& _colors,
                                 const glm::mat4& _modelTransform = glm::mat4(1.0f));
    };
}

#endif //GEOMHELPERS_H
