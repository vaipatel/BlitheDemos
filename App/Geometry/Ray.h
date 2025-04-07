#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

namespace blithe
{
    ///
    /// \brief Ray defined by an origin and a normalized direction
    ///
    struct Ray
    {
        glm::vec3 m_origin; //!< Ray's origin
        glm::vec3 m_dir;    //!< Ray's normalized direction
    };
}

#endif // RAY_H
