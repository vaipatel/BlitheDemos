#ifndef TRIBSPTREE_H
#define TRIBSPTREE_H

#include "Tri.h"
#include <vector>

namespace blithe
{
    class TriBSPTree
    {
    public:
        TriBSPTree();
        ~TriBSPTree();

        void AddTriangle(const Tri& _tri);
        static void Traverse(TriBSPTree* _root, const glm::vec3& _cameraPos,
                             std::vector<std::vector<Tri>>& _outTris);

        std::vector<Tri> m_coplanarTris; //!< Triangles coplanar to this node's plane, implicitly taken to be the plane of the first tri in this list.

    private:
        struct SplitResult
        {
            Tri m_t0;
            Tri m_t1;
            Tri m_t2;
            bool m_loneInFront;
        };
        static SplitResult SplitTriangle(const Tri& _tri,
                                         const std::array<float, 3>& _fs,
                                         const Tri& _splitter);
        void AddTriToFront(const Tri& _tri);
        void AddTriToBack(const Tri& _tri);
        float CalcImplicitFunc(const glm::vec3& _point);
        static glm::vec3 FindIntersectionWithTriPlane(const glm::vec3& _start,
                                                      const glm::vec3& _end,
                                                      const Tri& _tri);

        TriBSPTree* m_backTree = nullptr;   //!< The back subtree of tris where f <= 0
        TriBSPTree* m_frontTree = nullptr;  //!< The front subtree of tris where f > 0
    };
}

#endif // TRIBSPTREE_H
