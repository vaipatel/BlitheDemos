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

        std::vector<Tri> m_coplanarTris; //!< Triangles coplanar to this node's plane
        Plane* m_plane = nullptr;        //!< Node's plane

    private:
        ///
        /// \brief Result of splitting a poly (usually a triangle) with node planes in the tree.
        ///        Vector members help to pass the same SplitResult recursively down the tree.
        ///
        struct SplitResult
        {
            std::vector<Tri> m_frontTris; //!< Tris in front of the split plane
            std::vector<Tri> m_backTris;  //!< Tris behind the split plane
            std::vector<Tri> m_coplanarFrontTris; //!< Tris coplanar with the split plane
            std::vector<Tri> m_coplanarBackTris; //!< Coplanar tris but with normal backwards
        };

        static void SplitTriangle(const Tri& _tri, const Plane* _splitter, SplitResult& _res);
        void AddTriToFront(const Tri& _tri);
        void AddTriToBack(const Tri& _tri);
        static float CalcImplicitFunc(const glm::vec3& _point,
                                      const Plane* _plane,
                                      float _snapToZeroTol);
        static glm::vec3 FindIntersectionWithTriPlane(const glm::vec3& _start,
                                                      const glm::vec3& _end,
                                                      const Plane* _plane);

        TriBSPTree* m_backTree = nullptr;   //!< The back subtree of tris where f <= 0
        TriBSPTree* m_frontTree = nullptr;  //!< The front subtree of tris where f > 0
    };
}

#endif // TRIBSPTREE_H
