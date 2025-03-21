#ifndef CACHEDMESHOBJECT_H
#define CACHEDMESHOBJECT_H

#include "RenderObject.h"
#include "VBOVertexFormat.h"

#define MAX_CACHEDMESHOBJECT_NAME_LEN   128

namespace blithe
{
    class IGLBufferCache;
    struct Mesh;

    ///
    /// \brief Mesh Object that uses cached OpenGL buffers and arrays for its vertices.
    ///
    class CachedMeshObject : public RenderObject
    {
    public:
        CachedMeshObject(const char* _name,
                         const Mesh* _mesh,
                         IGLBufferCache* _glBufferCache);
        ~CachedMeshObject() override {}

        void Render() override;

    private:
        const Mesh* m_mesh = nullptr;               //!< Mesh data
        IGLBufferCache* m_glBufferCache = nullptr;  //!< Cache to use for OpenGL buffers and arrays
        char m_name[MAX_CACHEDMESHOBJECT_NAME_LEN]; //!< Name for this

        ///
        /// \brief Vertex format for the Mesh data
        ///
        VBOVertexFormat m_format_xyz_rgba_uv = {
            { {0, 3, GL_FLOAT, GL_FALSE, 0},                   // position
              {1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 3},   // color
              {2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 7} }, // texture coords
            sizeof(float) * 9
        };
    };
}

#endif // CACHEDMESHOBJECT_H
