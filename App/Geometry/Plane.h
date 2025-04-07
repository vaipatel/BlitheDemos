#ifndef PLANE_H
#define PLANE_H

#include <glm/glm.hpp>

namespace blithe
{
    ///
    /// \brief Plane defined by a normal n and signed distance to origin d as the locus of points x
    ///        such that n * x + d = 0
    ///
    struct Plane
    {
        glm::vec3 m_normal;
        float m_d;

        ///
        /// \brief Calculates the signed distance of _point from this plane.
        /// \param _point         - Point at which to evaluate implicit function
        /// \param _snapToZeroTol - Abs tolerance. f's value is snapped to zero if below this.
        /// \return Signed distance of _point from this
        ///
        float CalcSignedDist(const glm::vec3& _point,
                             float _snapToZeroTol = std::numeric_limits<float>::epsilon()) const
        {
            float val = glm::dot(m_normal, _point) + m_d;

            // Snap to zero if less than tolerance
            if ( std::abs(val) < _snapToZeroTol )
            {
                val = 0.0f;
            }

            return val;
        }
    };
}

#endif // PLANE_H
