#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include "VBOVertexFormat.h"
#include "Mesh.h"

namespace blithe
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

        void SetInstances(const std::vector<glm::mat4>& _transforms);

        void Render();

        const Mesh& GetMesh() const { return m_mesh; }

    private:
        void SetupMesh();
        void CleanUp();

        unsigned int m_vao;    //!< ID of Vertex Array Object holding the vertex layout
        unsigned int m_vbo;    //!< ID of Vertex Buffer Object holding the vertex data
        unsigned int m_ebo;    //!< ID of Element Buffer Object holding the mesh layout
        unsigned int m_ibo;    //!< ID of Vertex Buffer Object holding any instances data
        size_t m_numInstances; //!< Number of instances, equal to the number of transforms given for instancing
        Mesh m_mesh;           //!< The mesh geometry

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

#endif //MESHOBJECT_H
