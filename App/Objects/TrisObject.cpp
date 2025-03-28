#include "TrisObject.h"
#include <glad/glad.h>

namespace blithe
{
    /*!
     * \brief Constructor that creates the VBO and VAO for the given _tris soup.
     *
     * \param _tris - Soup (i.e. arbitrary list) of triangles
     */
    TrisObject::TrisObject(const std::vector<Tri>& _tris) :
        m_vao(0),
        m_vbo(0),
        m_numTris(_tris.size())
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(_tris.size() * sizeof(Tri)), _tris.data(), GL_STATIC_DRAW);

        for (const auto& attr : m_format_xyz_rgba_uv.m_attributes)
        {
            glEnableVertexAttribArray(attr.m_index);
            glVertexAttribPointer(attr.m_index,
                                  attr.m_size,
                                  attr.m_type,
                                  attr.m_normalized,
                                  m_format_xyz_rgba_uv.m_stride,
                                  reinterpret_cast<void*>(attr.m_offset));
        }

        glBindVertexArray(0);
    }

    /*!
     * \brief Destructor
     */
    TrisObject::~TrisObject()
    {
        CleanUp();
    }

    /*!
     * \brief Binds and draws the vertex arrays.
     */
    void TrisObject::Render()
    {
        glBindVertexArray(m_vao);
        GLsizei numVerts = static_cast<GLsizei>(m_numTris) * 3;
        glDrawArrays(GL_TRIANGLES, 0, numVerts);
        glBindVertexArray(0);
    }

    /*!
     * \brief Deletes the VAO and VBO.
     */
    void TrisObject::CleanUp()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
    }
}
