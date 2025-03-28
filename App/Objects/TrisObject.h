#ifndef TRISOBJECT_H
#define TRISOBJECT_H

#include "VBOVertexFormat.h"
#include "Tri.h"
#include <vector>

namespace blithe
{
    /*!
     * \brief Renderable triangle soup object.
     *        No optimizations are done to reduce the number of vertices.
     */
    class TrisObject
    {
    public:
        TrisObject(const std::vector<Tri>& _tris);
        ~TrisObject();

        void Render();

    private:
        void CleanUp();

        unsigned int m_vao; //!< ID of Vertex Array Object holding the vertex layout
        unsigned int m_vbo; //!< ID of Vertex Buffer Object holding the vertex data
        size_t m_numTris;   //!< Number of triangles

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

#endif // TRISOBJECT_H
