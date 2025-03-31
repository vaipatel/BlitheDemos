#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>

namespace blithe
{
    ///
    //// \brief Axis-Aligned Bounding Box defined by its minimum and maximum corners
    ///
    struct AABB
    {
        glm::vec3 m_min; //!< Minimum corner of the AABB (smallest x, y, z values)
        glm::vec3 m_max; //!< Maximum corner of the AABB (largest x, y, z values)
    };
}

#endif // AABB_H
