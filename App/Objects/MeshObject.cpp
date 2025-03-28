#include "MeshObject.h"
#include "BlitheAssert.h"
#include <glad/glad.h>

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

        // Goal: Set transformation matrices as an instance vertex attribute (with divisor 1)
        // The instance's matrix will be available in the next available layout location on the
        // vertex shader (so after the texture coords for our m_format_xyz_rgba_uv format).
        // We're going to upload the matrix as 4 vec4s. But from I understand, on the shader side we
        // can just compactly use "out mat4 InstanceMatrix".
        // (AFAIU, we could use 4 "out vec4 InstanceMatColN" lines consecutively where N=0,1,2,3.
        // But that's so much typing)
        const GLuint NumExistingAttrs = m_format_xyz_rgba_uv.m_attributes.size();
        for (GLuint colIdx = 0; colIdx < 4; colIdx++)
        {
            // Upload the columns after the existing vertex attribute layout locations
            GLuint attrIdx = NumExistingAttrs + colIdx;
            glEnableVertexAttribArray(attrIdx);
            glVertexAttribPointer(attrIdx, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(colIdx * sizeof(glm::vec4)));
        }

        // The divisor of 1 means we only change the attribute per instance, not per vertex
        for (GLuint colIdx = 0; colIdx < 4; colIdx++)
        {
            GLuint attrIdx = NumExistingAttrs + colIdx;
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
