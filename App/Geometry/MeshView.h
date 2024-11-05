#ifndef MESHVIEW_H
#define MESHVIEW_H

#include "MeshIterator.h"
#include "Tri.h"

namespace blithe
{
    struct Mesh;

    ///
    /// \brief Gives a "View" into a Mesh, such as getting its i'th triangle etc.
    ///        Separated out so that the Mesh struct itself can be a lightweight POD.
    ///
    class MeshView
    {
    public:
        explicit MeshView(const Mesh& _mesh);

        static Tri GetTriangleAtIndex(const Mesh& _mesh, size_t _index);
        size_t GetNumTriangles() const;
        Tri GetTriangle(size_t _index) const;

        using TriangleIterator = MeshIterator<Tri, Mesh>;

        TriangleIterator TriangleBegin() const;
        TriangleIterator TriangleEnd() const;

    private:
        static void SanityCheck(const Mesh& _mesh);

        const Mesh& m_mesh; //!< Const ref to Mesh we are a getting a "View" into
    };
}

#endif //MESHVIEW_H
