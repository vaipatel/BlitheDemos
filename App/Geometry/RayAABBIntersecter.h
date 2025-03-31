#ifndef RAYAABBINTERSECTER_H
#define RAYAABBINTERSECTER_H

#include <glm/glm.hpp>
#include <optional.hpp>

namespace blithe
{
    struct AABB;

    ///
    /// \brief Represents the result of a ray-AABB intersection
    ///
    struct RayIntersectionResult
    {
        glm::vec3 m_entryPt; //!< The point where the ray enters the AABB
        glm::vec3 m_exitPt;  //!< The point where the ray exits the AABB
        float m_tClose;      //!< Distance along the ray to the entry point
        float m_tFar;        //!< Distance along the ray to the exit point
    };

    ///
    /// \brief Utility class to perform ray-AABB intersection tests
    ///
    class RayAABBIntersecter
    {
    public:
        static tl::optional<RayIntersectionResult> Intersect(const glm::vec3& _rayOrigin,
                                                             const glm::vec3& _rayDir,
                                                             const AABB& _aabb);
    };
}

#endif // RAYAABBINTERSECTER_H
