#include "TrisObject.h"
#include "ShaderProgram.h"

#include <glad/glad.h>
#include <numeric>

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

        // (The vertex attrib layout below is trying to be clever and avoid describing the
        // layout for each attribute, instead trying to be "generic". I'm not sure how
        // worthwhile that is. Considering I have to assume they are floats anyway.)
        std::array<size_t, 3> numComponentsPerAttr = Vertex::GetNumComponentsPerAttribute();
        size_t totalNumComponents = std::accumulate(std::begin(numComponentsPerAttr), std::end(numComponentsPerAttr), 0ull);
        size_t offset = 0;
        for (GLuint attrIdx = 0; attrIdx < numComponentsPerAttr.size(); ++attrIdx)
        {
            GLint numComponents = static_cast<GLint>(numComponentsPerAttr[attrIdx]);
            glEnableVertexAttribArray(attrIdx);
            glVertexAttribPointer(attrIdx, numComponents, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(totalNumComponents * sizeof(float)), reinterpret_cast<void*>(offset));
            offset += static_cast<size_t>(numComponents) * sizeof(float);
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
