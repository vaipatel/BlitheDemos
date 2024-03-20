#include "MeshObject.h"
#include "BlitheAssert.h"
#include "ShaderProgram.h"
#include <glad/glad.h>
#include <numeric>

namespace BLE
{
    /*!
     * \brief Constructor. Creates the VAO, VBO and EBO for the _mesh.
     *
     * \param _mesh - The mesh geometry
     */
    MeshObject::MeshObject(const Mesh & _mesh) :
        m_vao(0),
        m_vbo(0),
        m_ebo(0),
        m_mesh(_mesh)
    {
        SetupMesh();
    }

    /*!
     * \brief Destructor
     */
    MeshObject::~MeshObject()
    {
        CleanUp();
    }

    /*!
     * \brief Binds and draws the 
     * 
     */
    void MeshObject::Render()
    {
        glBindVertexArray(m_vao);
        GLsizei numIndices = static_cast<GLsizei>(m_mesh.m_indices.size());
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    /*!
     * \brief Sets up mesh.
     */
    void MeshObject::SetupMesh()
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_mesh.m_vertices.size() * sizeof(Vertex)), m_mesh.m_vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_mesh.m_indices.size() * sizeof(unsigned int)), m_mesh.m_indices.data(), GL_STATIC_DRAW);

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
     * \brief Deletes the VAO, VBO and EBO.
     */
    void MeshObject::CleanUp()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
    }
}