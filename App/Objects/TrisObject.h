#ifndef TRISOBJECT_H
#define TRISOBJECT_H

#include "Tri.h"
#include <vector>

namespace IME
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
        void Draw();
        void CleanUp();

    private:
        unsigned int m_vao; //!< ID of Vertex Array Object holding the vertex layout
        unsigned int m_vbo; //!< ID of Vertex Buffer Object holding the vertex data
        size_t m_numTris;   //!< Number of triangles
    };
}

#endif // TRISOBJECT_H
