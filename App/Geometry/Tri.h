#ifndef TRI_H
#define TRI_H

#include "Vertex.h"

namespace blithe
{
    /*!
     * Represents a triangle composed of 3 vertices.
     *
     */
    struct Tri
    {
        Vertex m_v0; //!< First vertex
        Vertex m_v1; //!< Second vertex
        Vertex m_v2; //!< Third vertex

        void CalcNormalAndD(glm::vec3& _normal, float& _d) const
        {
            _normal = glm::cross(m_v1.m_pos - m_v0.m_pos, m_v2.m_pos - m_v0.m_pos);
            _normal = glm::normalize(_normal);
            _d = -glm::dot(_normal, m_v0.m_pos);
        }

        float Area() const
        {
            return 0.5f * glm::length(glm::cross(m_v1.m_pos - m_v0.m_pos, m_v2.m_pos - m_v0.m_pos));
        }
    };
}

#endif // TRI_H
