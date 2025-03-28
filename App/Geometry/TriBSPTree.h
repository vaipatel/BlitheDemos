#ifndef TRIBSPTREE_H
#define TRIBSPTREE_H

#include "Tri.h"
#include <array>
#include <stack>
#include <vector>

namespace blithe
{
    class TriBSPTree;

    //! Entry in stack for data recursion
    struct TriBSPTreeStackEntry
    {
        TriBSPTree* m_node = nullptr;          //!< Node being processed
        bool m_firstSubtreeProcessed = false;  //!< Whether the first subtree has been processed
        TriBSPTree* m_secondSubtree = nullptr; //!< Root of second subtree to process
    };

    class TriBSPTree
    {
    public:
        TriBSPTree();
        ~TriBSPTree();

        void AddTriangle(const Tri& _tri);
        static void TraverseRecursively(TriBSPTree* _root,
                                        const glm::vec3& _cameraPos,
                                        std::vector<std::vector<Tri>>& _outTris);
        static void TraverseWithStackIterative(TriBSPTree* _root,
                                               const glm::vec3& _cameraPos,
                                               std::vector<std::vector<Tri>>& _outTris,
                                               std::stack<TriBSPTreeStackEntry>& _outNodeStack,
                                               int _maxNodes);

        static void CountTotalNumTris(TriBSPTree* _root, size_t& _outNumTris);

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
        float CalcImplicitFunc(const glm::vec3& _point, float _snapToZeroTol);
        static glm::vec3 FindIntersectionWithTriPlane(const glm::vec3& _start,
                                                      const glm::vec3& _end,
                                                      const Tri& _tri);

        TriBSPTree* m_backTree = nullptr;   //!< The back subtree of tris where f <= 0
        TriBSPTree* m_frontTree = nullptr;  //!< The front subtree of tris where f > 0
    };
}

#endif // TRIBSPTREE_H
