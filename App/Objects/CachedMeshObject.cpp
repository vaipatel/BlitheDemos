#include "CachedMeshObject.h"
#include <cstring>
#include "BlitheAssert.h"
#include "IGLBufferCache.h"
#include "Mesh.h"

namespace blithe
{
    ///
    /// \brief Constructor
    ///
    /// \param _name          - Name for this. Must be unique across all CachedMeshObjects.
    /// \param _mesh          - Mesh data
    /// \param _glBufferCache - Cache for OpenGL buffers and arrays
    ///
    CachedMeshObject::CachedMeshObject(const char* _name,
                                       const Mesh* _mesh,
                                       IGLBufferCache* _glBufferCache)
        : m_mesh(_mesh),
          m_glBufferCache(_glBufferCache)
    {
        ASSERT(std::strlen(_name) < MAX_CACHEDMESHOBJECT_NAME_LEN, "name too long");
        std::strncpy(m_name, _name, MAX_CACHEDMESHOBJECT_NAME_LEN);
    }

    ///
    /// \brief Gets the VAO from the OpenGL buffer cache (uploading the VBO, EBO and VAO as needed),
    ///        binds the VAO and submits the draw call to draw the triangles.
    ///
    void CachedMeshObject::Render()
    {
        GLuint vbo = m_glBufferCache->GetVertexBuffer(m_name,
                                                      m_mesh->m_vertices.data(),
                                                      m_mesh->m_vertices.size() * sizeof(Vertex));

        GLuint ebo = m_glBufferCache->GetIndexBuffer(m_name,
                                                     m_mesh->m_indices);

        GLuint vao = m_glBufferCache->GetVertexArray(m_name,
                                                     vbo,
                                                     ebo,
                                                     m_format_xyz_rgba_uv);

        glBindVertexArray(vao);
        GLsizei numIndices = static_cast<GLsizei>(m_mesh->m_indices.size());
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
        glBindVertexArray(0);
    }
}
