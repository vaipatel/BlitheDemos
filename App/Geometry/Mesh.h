#ifndef MESH_H
#define MESH_H

#include "Vertex.h"
#include <vector>

namespace IME
{
    /*!
     * \brief Simple mesh class describing a list of vertices and indices into the
     *        vertices.
     *        
     *        The indices should be consecutive triples describing the triangle faces
     *        in CCW order.
     */
    struct Mesh
    {
        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
    };
}

#endif //MESH_H