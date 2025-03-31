#include "RayAABBIntersecter.h"
#include "AABB.h"
#include <iostream>

namespace blithe
{

    ///
    /// \brief Uses the Slab method (https://en.wikipedia.org/wiki/Slab_method) for intersecting the
    ///        ray defined by _rayOrigin and _rayDir with the given _aabb
    ///
    /// \param _rayOrigin - Origin of the ray
    /// \param _rayDir    - Normalized direction of the ray
    /// \param _aabb      - AABB to intersect with
    ///
    /// \return Optional RayIntersectionResult if the ray intersects the AABB
    ///
    tl::optional<RayIntersectionResult> RayAABBIntersecter::Intersect(
            const glm::vec3& _rayOrigin,
            const glm::vec3& _rayDir,
            const AABB& _aabb)
    {
        tl::optional<RayIntersectionResult> result;

        float tnear = -std::numeric_limits<float>::infinity();
        float tfar = std::numeric_limits<float>::infinity();
        bool intersects = true;
        for ( int i = 0; i < 3; i++ )
        {
            // Check if ray is parallel to the ith slab by checking ray dir's ith component.
            // (Example: if rayDir were locked in the yz plane, rayDir's x component would be 0.)
            if ( std::abs(_rayDir[i]) < 1e-8f )
            {
                // The ray is parallel to the ith slab. Check if origin's ith coord is inside.
                if (_rayOrigin[i] < _aabb.m_min[i] || _rayOrigin[i] > _aabb.m_max[i])
                {
                    // So the ray is completely outside the box
                    intersects = false;
                    // std::cout << "ray parallel and outside" << std::endl;
                }
            }
            else
            {
                float tmin_i = (_aabb.m_min[i] - _rayOrigin[i]) / _rayDir[i];
                float tmax_i = (_aabb.m_max[i] - _rayOrigin[i]) / _rayDir[i];

                float tnear_i = std::fminf(tmin_i, tmax_i);
                float tfar_i = std::fmaxf(tmin_i, tmax_i);

                tnear = std::fmaxf(tnear, tnear_i);
                tfar = std::fminf(tfar, tfar_i);
                // Intersection can only happen if tnear <= tfar
                if ( tnear > tfar )
                {
                    intersects = false;
                    // std::cout << "tnear > tfar" << std::endl;
                }
            }
        }

        if ( intersects )
        {
            glm::vec3 entryPt = _rayOrigin + tnear * _rayDir;
            glm::vec3 exitPt = _rayOrigin + tfar * _rayDir;
            result = {entryPt, exitPt, tnear, tfar};
        }

        return result;
    }
}
