#include "RayAABBIntersecter.h"
#include "AABB.h"
// #include <iostream>

namespace blithe
{

    ///
    /// \brief Uses the Slab method (https://en.wikipedia.org/wiki/Slab_method) for intersecting the
    ///        ray _ray with the given AABB _aabb
    ///
    /// \param _ray  - Ray (Origin + Normalized direction)
    /// \param _aabb - AABB to intersect with
    ///
    /// \return Optional RayIntersectionResult if the ray intersects the AABB
    ///
    tl::optional<RayIntersectionResult> RayAABBIntersecter::Intersect(
            const Ray& _ray,
            const AABB& _aabb)
    {
        tl::optional<RayIntersectionResult> result;

        float tnear = -std::numeric_limits<float>::infinity();
        float tfar = std::numeric_limits<float>::infinity();
        bool intersects = true;
        for ( int i = 0; i < 3; i++ )
        {
            // Check if ray is parallel to the ith slab by checking dir's ith component.
            // (Example: if dir were locked in the yz plane, dir's x component would be 0.)
            if ( std::abs(_ray.m_dir[i]) < 1e-8f )
            {
                // The ray is parallel to the ith slab. Check if origin's ith coord is inside.
                if (_ray.m_origin[i] < _aabb.m_min[i] || _ray.m_origin[i] > _aabb.m_max[i])
                {
                    // So the ray is completely outside the box
                    intersects = false;
                    // std::cout << "ray parallel and outside" << std::endl;
                }
            }
            else
            {
                float tmin_i = (_aabb.m_min[i] - _ray.m_origin[i]) / _ray.m_dir[i];
                float tmax_i = (_aabb.m_max[i] - _ray.m_origin[i]) / _ray.m_dir[i];

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
            glm::vec3 entryPt = _ray.m_origin + tnear * _ray.m_dir;
            glm::vec3 exitPt = _ray.m_origin + tfar * _ray.m_dir;
            result = {entryPt, exitPt, tnear, tfar};
        }

        return result;
    }
}
