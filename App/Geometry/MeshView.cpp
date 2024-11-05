#include "MeshView.h"
#include "BlitheAssert.h"
#include "Mesh.h"
#include <vector>

namespace blithe
{
    ///
    /// \brief Constructor
    ///
    /// \param _mesh - Const ref to Mesh we are a getting a "View" into
    ///
    MeshView::MeshView(const Mesh& _mesh)
        : m_mesh(_mesh)
    {
    }

    ///
    /// \brief Static function to access the _index'th triangle in _mesh.
    ///
    /// \param _mesh  - Mesh to lookup triangle
    /// \param _index - Index of desired triangle
    ///
    /// \return _index'th triangle in _mesh
    ///
    Tri MeshView::GetTriangleAtIndex(const Mesh& _mesh, size_t _index)
    {
        SanityCheck(_mesh);

        Tri tri;
        size_t startIdx = _index*3;
        tri.m_v0 = _mesh.m_vertices[_mesh.m_indices[startIdx + 0]];
        tri.m_v1 = _mesh.m_vertices[_mesh.m_indices[startIdx + 1]];
        tri.m_v2 = _mesh.m_vertices[_mesh.m_indices[startIdx + 2]];

        return tri;
    }

    ///
    /// \brief Returns the total number of triangles in the mesh.
    ///        Assuming the indices are a list of triples, this is just indices.size()/3.
    ///
    /// \return Total number of triangles in the mesh
    ///
    size_t MeshView::GetNumTriangles() const
    {
        SanityCheck(m_mesh);

        return m_mesh.m_indices.size() / 3;
    }

    ///
    /// \brief Gets the _index'th triangle in the mesh.
    ///
    /// \param _index - Index of desired triangle
    ///
    /// \return _index'th triangle in the mesh
    ///
    Tri MeshView::GetTriangle(size_t _index) const
    {
        return GetTriangleAtIndex(m_mesh, _index);
    }

    ///
    /// \brief Iterator begin for iterating over triangles
    ///
    /// \return TriangleIterator begin
    ///
    MeshView::TriangleIterator MeshView::TriangleBegin() const
    {
        return TriangleIterator(m_mesh, 0, GetTriangleAtIndex);
    }

    ///
    /// \brief Iterator end for iterating over triangles
    ///
    /// \return TriangleIterator end
    ///
    MeshView::TriangleIterator MeshView::TriangleEnd() const
    {
        return TriangleIterator(m_mesh, GetNumTriangles(), GetTriangleAtIndex);
    }


    void MeshView::SanityCheck(const Mesh& _mesh)
    {
        ASSERT(_mesh.m_indices.size() % 3 == 0, "Indices must be a flattened list of triples describing faces. Got Num Indices = " << _mesh.m_indices.size());
        //ASSERT(m_vertices.size() >= 3 , "Must have atleast 3 vertices to make a mesh. Got " << m_vertices.size());
    }
}
