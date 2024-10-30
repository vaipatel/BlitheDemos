#include "MeshObject.h"
#include "BlitheAssert.h"
#include "ShaderProgram.h"
#include <glad/glad.h>
#include <numeric>

namespace blithe
{
    /*!
     * \brief Constructor. Creates the VAO, VBO and EBO for the _mesh.
     *
     * \param _mesh - The mesh geometry
     */
    MeshObject::MeshObject(const Mesh& _mesh) :
        m_vao(0),
        m_vbo(0),
        m_ebo(0),
        m_ibo(0),
        m_numInstances(0),
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

    ///
    /// \brief Recreates the instance buffer and uploads the matrix _transforms so that the mesh
    ///        can be rendered with instanced rendering.
    ///
    /// \param _transforms - Transforms to use for rendering instances of the mesh. Typically these
    ///                      are the model matrices, but generally the vertex shader must know what
    ///                      to do with them.
    ///
    void MeshObject::SetInstances(const std::vector<glm::mat4>& _transforms)
    {
        ASSERT(m_vao != 0, "The MeshObject needs to have been setup before adding instances");
        ASSERT(_transforms.size() > 0, "Must provide a non-zero number of transforms for instancing.");

        if ( m_ibo != 0 )
        {
            ASSERT(m_numInstances == 0, "Instance buffer previously generated even though num transforms was 0.");
            glDeleteBuffers(1, &m_ibo);
        }

        m_numInstances = _transforms.size();

        // Bind the same vertex array object as we do for the setup and upload of the mesh
        glBindVertexArray(m_vao);

        // Generate a buffer for the instances data and upload the instances data
        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(_transforms.size() * sizeof(glm::mat4)), _transforms.data(), GL_STATIC_DRAW);

        // Adapted from learnopengl:
        // set transformation matrices as an instance vertex attribute (with divisor 1)
        // set attribute pointers for matrix (4 times vec4)
        std::array<size_t, 3> numComponentsPerAttr = Vertex::GetNumComponentsPerAttribute();
        const GLuint StartAttrIdx = numComponentsPerAttr.size();
        for (GLuint idx = 0; idx < 4; idx++)
        {
            GLuint attrIdx = StartAttrIdx + idx;
            glEnableVertexAttribArray(attrIdx);
            glVertexAttribPointer(attrIdx, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(idx * sizeof(glm::vec4)));
        }

        // The divisor of 1 means we only change the attribute per instance, not per vertex
        for (GLuint idx = 0; idx < 4; idx++)
        {
            GLuint attrIdx = StartAttrIdx + idx;
            glVertexAttribDivisor(attrIdx, 1);
        }

        glBindVertexArray(0);

    }

    /*!
     * \brief Binds and draws the 
     * 
     */
    void MeshObject::Render()
    {
        glBindVertexArray(m_vao);
        GLsizei numIndices = static_cast<GLsizei>(m_mesh.m_indices.size());
        if ( m_ibo == 0 )
        {
            glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
        }
        else
        {
            glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT,
                                    reinterpret_cast<void*>(0),
                                    static_cast<GLsizei>(m_numInstances));
        }
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
        if ( m_ibo != 0 )
        {
            glDeleteBuffers(1, &m_ibo);
        }
        m_numInstances = 0;
    }
}
