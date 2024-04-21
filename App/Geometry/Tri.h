#ifndef TRI_H
#define TRI_H

#include "Vertex.h"

namespace blithe
{
    /*!
     * Represents a triangle composed of 3 vertices.
     *
     */
    struct Tri
    {
        Vertex m_v0; //!< First vertex
        Vertex m_v1; //!< Second vertex
        Vertex m_v2; //!< Third vertex
    };
}

#endif // TRI_H
