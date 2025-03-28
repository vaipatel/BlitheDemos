#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

namespace blithe
{
    /*!
     * \brief Represents a vertex with position, color and texture coordinate information.
     */
    struct Vertex
    {
        glm::vec3 m_pos;       //!< Position Coordinates
        glm::vec4 m_color;     //!< Color
        glm::vec2 m_texCoords; //!< Texture Coordinates
    };
}

#endif // VERTEX_H
