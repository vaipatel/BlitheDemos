#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include "Mesh.h"

namespace BLE
{
    /*!
     * \brief Renderable triangle mesh.
     *        Takes a Mesh geometry which has a list of vertices and indices describing
     *        the faces as vertex triples (in CCW order). So this can use EBOs and
     *        avoid repeating vertex data as opposed to TrisObject.
     */
    class MeshObject
    {
    public:
        explicit MeshObject(const Mesh& _mesh);
        ~MeshObject();

        void Render();

    private:
        void SetupMesh();
        void CleanUp();

        unsigned int m_vao; //!< ID of Vertex Array Object holding the vertex layout
        unsigned int m_vbo; //!< ID of Vertex Buffer Object holding the vertex data
        unsigned int m_ebo; //!< ID of Element Buffer Object holding the mesh layout
        Mesh m_mesh;        //!< The mesh geometry
    };
}

#endif //MESHOBJECT_H