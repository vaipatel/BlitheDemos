#ifndef VBOVERTEXFORMAT_H
#define VBOVERTEXFORMAT_H

#include <glad/glad.h>
#include <vector>

namespace blithe
{
    ///
    /// \brief Represents attributes of a specific vertex in a vertex buffer object
    ///
    struct VBOVertexAttribute
    {
        GLuint m_index;         //!< Attribute location (e.g., 0 = position, 1 = normal)
        GLint m_size;           //!< Number of components (e.g., 3 for vec3)
        GLenum m_type;          //!< GL_FLOAT, GL_INT, etc.
        GLboolean m_normalized; //!< Whether component values are normalized
        size_t m_offset;        //!< Offset in the vertex struct
    };

    ///
    /// \brief Describes the layout of a vertex in a vertex buffer object
    ///
    struct VBOVertexFormat
    {
        VBOVertexFormat(std::initializer_list<VBOVertexAttribute> _attrs, size_t _strideBytes)
            : m_attributes(_attrs), m_stride(_strideBytes)
        {
        }

        std::vector<VBOVertexAttribute> m_attributes; //!< Attribute definitions (position, normal, etc.)
        size_t m_stride; //!< Stride in bytes (total size of vertex struct)
    };
}

#endif // VBOVERTEXFORMAT_H
