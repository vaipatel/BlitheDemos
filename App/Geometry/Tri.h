#ifndef TRI_H
#define TRI_H

#include "Plane.h"
#include "Vertex.h"

namespace blithe
{
    ///
    /// \brief Represents a triangle composed of 3 vertices
    ///
    struct Tri
    {
        Vertex m_v0; //!< First vertex
        Vertex m_v1; //!< Second vertex
        Vertex m_v2; //!< Third vertex

        ///
        /// \brief Calculates the normal and signed distance to origin for the plane of this triangle
        /// \param _normal - (out) Populated with plane normal
        /// \param _d      - (out) Populated with plane signed distance
        ///
        void CalcNormalAndD(glm::vec3& _normal, float& _d) const
        {
            _normal = glm::cross(m_v1.m_pos - m_v0.m_pos, m_v2.m_pos - m_v0.m_pos);
            _normal = glm::normalize(_normal);
            _d = -glm::dot(_normal, m_v0.m_pos);
        }

        ///
        /// \brief Calculates the plane of this triangle
        /// \return Plane of this triangle
        ///
        Plane CalcPlane() const
        {
            glm::vec3 normal;
            float d;
            CalcNormalAndD(normal, d);
            return Plane{normal, d};
        }

        ///
        /// \brief Calculates the area of this triangle
        /// \return Area of this triangle
        ///
        float Area() const
        {
            return 0.5f * glm::length(glm::cross(m_v1.m_pos - m_v0.m_pos, m_v2.m_pos - m_v0.m_pos));
        }
    };
}

#endif // TRI_H
