#include "TriBSPTree.h"
#include "BlitheAssert.h"
#include <vector>

#define MIN_F_VAL 1e-5f

namespace blithe
{
    ///
    /// \brief Default constructor
    ///
    TriBSPTree::TriBSPTree()
    {
    }

    ///
    /// \brief Destructor
    ///
    TriBSPTree::~TriBSPTree()
    {
        m_coplanarTris.clear();
        delete m_plane;
        delete m_backTree;
        delete m_frontTree;
    }

    ///
    /// \brief Inserts triangle _tri into this BSP Tree recursively.
    ///
    ///        SplitTriangle() which does the important work of determining _tri's spatial
    ///        relationship to each node as this function recurses i.e. whether _tri is COPLANAR,
    ///        IN FRONT OF, BEHIND or INTERSECTING the plane of the node that we have recursed down
    ///        to, and splitting _tri if needed.
    ///
    ///        I took inspiration from the pseudo-code in Algorithm 3 of the below cited article
    ///        "Constructive Solid Geometry Using BSP Tree". (I don't know if I did a good job of
    ///        understanding and translating it to code, especially the splitting code, but it
    ///        seems to work.)
    ///
    /// \cite Segura, C.D., Stine, T., & Yang, J. (2013). Constructive Solid Geometry Using BSP Tree.
    ///
    /// \param _tri - Triangle to insert
    ///
    void TriBSPTree::AddTriangle(const Tri& _tri)
    {
        // If we haven't inserted any tris into this node yet, do so now and return.
        if ( m_coplanarTris.empty() )
        {
            m_coplanarTris.push_back(_tri);
            Plane plane = _tri.CalcPlane();
            m_plane = new Plane();
            m_plane->m_normal = plane.m_normal;
            m_plane->m_d = plane.m_d;
            return;
        }

        SplitResult splitRes;
        SplitTriangle(_tri, m_plane, splitRes);
        // Either _tri is coplanar with this node's plane ..
        if ( !splitRes.m_coplanarFrontTris.empty() || !splitRes.m_coplanarBackTris.empty() )
        {
            m_coplanarTris.push_back(_tri);
            return;
        }
        // .. else _tri is behind this node's plane ..
        else if ( !splitRes.m_backTris.empty() )
        {
            AddTriToBack(_tri);
            return;
        }
        // .. else _tri is in front of this node's plane ..
        else if ( !splitRes.m_frontTris.empty() )
        {
            AddTriToFront(_tri);
            return;
        }
        // .. else _tri intersects the plane, so we need to split it.
        else
        {
            ASSERT(splitRes.m_frontTris.size() == 1 && splitRes.m_backTris.size() == 2 ||
                   splitRes.m_frontTris.size() == 2 && splitRes.m_backTris.size() == 1,
                   "Poor split condition detection");

            // std::cout << std::fixed << "t0 area = " << splitTris.m_t0.Area() << ", t1 area = " << splitTris.m_t1.Area() << ", t2 area = " << splitTris.m_t2.Area() << std::endl;
            if ( splitRes.m_frontTris.size() == 1 )
            {
                AddTriToFront(splitRes.m_frontTris[0]);
                AddTriToBack(splitRes.m_backTris[0]);
                AddTriToBack(splitRes.m_backTris[1]);
            }
            else
            {
                AddTriToBack(splitRes.m_backTris[0]);
                AddTriToFront(splitRes.m_frontTris[0]);
                AddTriToFront(splitRes.m_frontTris[1]);
            }
        }
    }

    ///
    /// \brief Traverses the BSP tree starting at _root such that triangles in the output _outTris
    ///        are priority listed from farthest to closest w.r.t. _cameraPos.
    ///
    ///        The purpose here is to get a back-to-front listing of triangles w.r.t. _cameraPos
    ///        which when rendered in that order would allow us to render the scene with alpha
    ///        blending to achieve transparency effects.
    ///
    /// \cite Fuchs, H., Kedem, Z.M., & Naylor, B.F. (1980). On visible surface generation by a
    ///       priori tree structures. International Conference on Computer Graphics and Interactive
    ///       Techniques.
    /// \cite Bsp Tree Frequently asked questions (FAQ). BSP Tree FAQ. (n.d.).
    ///       https://people.eecs.berkeley.edu/~jrs/274s19/bsptreefaq.html The relevant bit is under
    ///       "Pseudo C++ code example" in section 9. HOW DO YOU REMOVE HIDDEN SURFACES WITH A BSP
    ///       TREE?
    ///
    /// \param _root      - Root of tree
    /// \param _cameraPos - Position (eye) of camera to w.r.t. which a back-to-front ordering of
    ///                     polygons is desired
    /// \param _outTris   - (out) Back-to-front ordering of polygons (specifically triangles)
    ///
    void TriBSPTree::TraverseRecursively(TriBSPTree* _root,
                                         const glm::vec3& _cameraPos,
                                         std::vector<std::vector<Tri>>& _outTris)
    {
        if ( !_root ) return;

        // Classify _cameraPos w.r.t. _root
        float d = CalcImplicitFunc(_cameraPos, _root->m_plane, MIN_F_VAL);

        // As is stated in "IMAGE GENERATION PHASE" of "On visible surface generation by a
        // priori tree structures":
        //   Specifically, we are interested in the side (positive or negative) of the node's
        //   polygon where the current viewing position is located. Let's call the two sides the
        //   "containing" side and the "other" side. The traversal for a back-to-front ordering
        //   is 1) the "other" side, 2) the node, and 3) the "containing" side.
        if ( d > 0 )
        {
            // _cameraPos is in front, so start with the back tree
            TraverseRecursively(_root->m_backTree, _cameraPos, _outTris);
            _outTris.push_back(_root->m_coplanarTris);
            TraverseRecursively(_root->m_frontTree, _cameraPos, _outTris);
        }
        else if ( d < 0 )
        {
            // _cameraPos is in the back, so start with the front tree
            TraverseRecursively(_root->m_frontTree, _cameraPos, _outTris);
            _outTris.push_back(_root->m_coplanarTris);
            TraverseRecursively(_root->m_backTree, _cameraPos, _outTris);
        }
        else
        {
            // unsure, start with the front tree
            TraverseRecursively(_root->m_frontTree, _cameraPos, _outTris);
            TraverseRecursively(_root->m_backTree, _cameraPos, _outTris);
        }
    }

    ///
    /// \brief Like Traverse(), but using data recursion and a _maxNodes to limit the number of
    ///        nodes processed at once.
    ///
    ///        The recursion stack is stateful so successive calls will pick up from where the last
    ///        call ended. If the stack is empty this will just do it all over again. I don't have a
    ///        good way to let the caller know when to stop calling this. Perhaps they can do
    ///        while ( !started && !_outNodeStack.empty() ) { started = true; // Traverse }
    ///
    /// \param _root         - Root of tree
    /// \param _cameraPos    - Position (eye) of camera to w.r.t. which a back-to-front ordering of
    ///                        polygons is desired
    /// \param _outTris      - (out) Back-to-front ordering of polygons (specifically triangles)
    /// \param _outNodeStack - (in/out) Modifiable stack provided by caller for iterative traversal
    ///                                 of upto _maxNodes at a time
    /// \param _maxNodes     - Max number of nodes processed at a time
    ///
    void TriBSPTree::TraverseWithStackIterative(TriBSPTree* _root,
                                                const glm::vec3& _cameraPos,
                                                std::vector<std::vector<Tri>>& _outTris,
                                                std::stack<TriBSPTreeStackEntry>& _outNodeStack,
                                                int _maxNodes)
    {
        // If we're given a root ..
        if ( _root )
        {
            // and if the stack is empty, initialize it with the root node.
            if ( _outNodeStack.empty() )
            {
                _outNodeStack.push({_root, false, nullptr});
            }
        }
        // Else just process _outNodeStack.

        while ( !_outNodeStack.empty() && _maxNodes > 0 )
        {
            TriBSPTreeStackEntry& currentEntry = _outNodeStack.top();
            TriBSPTree* currentNode = currentEntry.m_node;

            if ( currentEntry.m_firstSubtreeProcessed )
            {
                // First subtree has been processed, so now we can add the coplanar triangles and
                // then the second subtree.
                _outTris.push_back(currentNode->m_coplanarTris);
                _outNodeStack.pop();
                _maxNodes--;
                if ( currentEntry.m_secondSubtree )
                {
                    _outNodeStack.push({currentEntry.m_secondSubtree, false, nullptr});
                }
            }
            else
            {
                // Mark that the first subtree will now be processed
                currentEntry.m_firstSubtreeProcessed = true;

                // Classify _cameraPos w.r.t. currentNode
                float d = CalcImplicitFunc(_cameraPos, currentNode->m_plane, MIN_F_VAL);

                // As is stated in "IMAGE GENERATION PHASE" of "On visible surface generation by a
                // priori tree structures":
                //   Specifically, we are interested in the side (positive or negative) of the node's
                //   polygon where the current viewing position is located. Let's call the two sides the
                //   "containing" side and the "other" side. The traversal for a back-to-front ordering
                //   is 1) the "other" side, 2) the node, and 3) the "containing" side.
                if ( d > 0 )
                {
                    // _cameraPos is in front, so start with the back tree
                    if ( currentNode->m_backTree )
                    {
                        _outNodeStack.push({currentNode->m_backTree, false, nullptr});
                    }
                    currentEntry.m_secondSubtree = currentNode->m_frontTree;
                }
                else if ( d < 0 )
                {
                    // _cameraPos is in the back, so start with the front tree
                    if ( currentNode->m_frontTree )
                    {
                        _outNodeStack.push({currentNode->m_frontTree, false, nullptr});
                    }
                    currentEntry.m_secondSubtree = currentNode->m_backTree;
                }
                else
                {
                    // unsure, start with the front tree
                    if ( currentNode->m_frontTree )
                    {
                        _outNodeStack.push({currentNode->m_frontTree, false, nullptr});
                    }
                    currentEntry.m_secondSubtree = currentNode->m_backTree;
                }
            }
        }
    }

    ///
    /// \brief Counts the total number of triangles. Does a DFS to walk the tree at _root.
    ///
    /// \param _root       - Tree root
    /// \param _outNumTris - (out) Total number of triangles
    ///
    void TriBSPTree::CountTotalNumTris(TriBSPTree* _root, size_t& _outNumTris)
    {
        if ( !_root ) return;

        CountTotalNumTris(_root->m_backTree, _outNumTris);
        //std::cout << ", " << _root->m_coplanarTris.size();
        _outNumTris += _root->m_coplanarTris.size();
        CountTotalNumTris(_root->m_frontTree, _outNumTris);
    }

    ///
    /// \brief Splits the given triangle _tri using the plane _splitter.
    ///
    ///        As mentioned in AddTriangle(), I took inspiration from the pseudo-code in Algorithm 3
    ///        of the below cited article "Constructive Solid Geometry Using BSP Tree".
    ///
    /// \cite Segura, C.D., Stine, T., & Yang, J. (2013). Constructive Solid Geometry Using BSP Tree.
    ///
    /// \param _tri       - Triangle to split
    /// \param _splitter  - Splitting plane
    /// \param _res       - (out) Result of split
    ///
    void TriBSPTree::SplitTriangle(const Tri& _tri, const Plane* _splitter, SplitResult& _res)
    {
        float fa = CalcImplicitFunc(_tri.m_v0.m_pos, _splitter, MIN_F_VAL);
        float fb = CalcImplicitFunc(_tri.m_v1.m_pos, _splitter, MIN_F_VAL);
        float fc = CalcImplicitFunc(_tri.m_v2.m_pos, _splitter, MIN_F_VAL);

        // If the implicit function evaluates to 0 for all 3 vertices of _tri, we classify _tri
        // as being coplanar with this node's plane ..
        if ( fa == 0.0f && fb == 0.0f && fc == 0.0f )
        {
            if ( glm::dot(_tri.CalcPlane().m_normal, _splitter->m_normal) > 0.0f )
            {
                _res.m_coplanarFrontTris.push_back(_tri);
                //_res.m_frontTris.push_back(_tri);
            }
            else
            {
                _res.m_coplanarBackTris.push_back(_tri);
                //_res.m_backTris.push_back(_tri);
            }
            return;
        }
        // .. else if the implicit function evaluates to <= 0, we classify _tri as being behind
        // this node's plane ..
        else if ( fa <= 0.0f && fb <= 0.0f && fc <= 0.0f )
        {
            _res.m_backTris.push_back(_tri);
            return;
        }
        // .. else we classify _tri as being in front of this node's plane ..
        else if ( fa >= 0.0f && fb >= 0.0f && fc >= 0.0f )
        {
            _res.m_frontTris.push_back(_tri);
            return;
        }
        // .. else _tri intersects the plane, so we need to split it.
        std::array<float, 3> fs {fa, fb, fc};

        std::vector<size_t> frontVertIdxes;
        std::vector<size_t> backVertIdxes;
        for ( size_t i = 0; i < fs.size(); i++ )
        {
            if ( fs[i] <= 0.0f )
            {
                backVertIdxes.push_back(i);
            }
            else
            {
                frontVertIdxes.push_back(i);
            }
        }
        bool oneBack = backVertIdxes.size() == 1 && frontVertIdxes.size() == 2;
        bool oneFront = backVertIdxes.size() == 2 && frontVertIdxes.size() == 1;
        if ( !(oneBack || oneFront) )
        {
            ASSERT(false, "Poor detection of triangle split condition. backVertIdxes.size() == " << backVertIdxes.size() << " frontVertIdxes.size() == " << frontVertIdxes.size());
        }

        // Get permuted vertices starting at the lone one off to one side
        std::vector<Vertex> triVerts = {_tri.m_v0, _tri.m_v1, _tri.m_v2};
        size_t startIdx = 0;
        if ( oneBack )
        {
            startIdx = backVertIdxes[0];
        }
        else
        {
            startIdx = frontVertIdxes[0];
        }
        std::vector<Vertex> rearrangedVerts = {triVerts[(startIdx+0)%3],
                                               triVerts[(startIdx+1)%3],
                                               triVerts[(startIdx+2)%3]};

        glm::vec3 intersectionA = FindIntersectionWithTriPlane(rearrangedVerts[0].m_pos,
                                                               rearrangedVerts[1].m_pos,
                                                               _splitter);
        glm::vec3 intersectionB = FindIntersectionWithTriPlane(rearrangedVerts[0].m_pos,
                                                               rearrangedVerts[2].m_pos,
                                                               _splitter);

        // Tri t0 = {r0, A , B}
        Tri t0;
        t0.m_v0 = rearrangedVerts[0];
        t0.m_v1 = {intersectionA, rearrangedVerts[0].m_color, rearrangedVerts[1].m_texCoords};
        t0.m_v2 = {intersectionB, rearrangedVerts[0].m_color, rearrangedVerts[2].m_texCoords};

        // Tri t1 = {A, r1, r2}
        Tri t1;
        t1.m_v0 = {intersectionA, rearrangedVerts[1].m_color, rearrangedVerts[0].m_texCoords};
        t1.m_v1 = rearrangedVerts[1];
        t1.m_v2 = rearrangedVerts[2];

        // Tri t2 = {A, r2, B}
        Tri t2;
        t2.m_v0 = {intersectionA, rearrangedVerts[2].m_color, rearrangedVerts[1].m_texCoords};
        t2.m_v1 = rearrangedVerts[2];
        t2.m_v2 = {intersectionB, rearrangedVerts[2].m_color, rearrangedVerts[0].m_texCoords};

        if ( oneFront )
        {
            _res.m_frontTris.push_back(t0);
            _res.m_backTris.push_back(t1);
            _res.m_backTris.push_back(t2);
        }
        else
        {
            _res.m_backTris.push_back(t0);
            _res.m_frontTris.push_back(t1);
            _res.m_frontTris.push_back(t2);
        }

        return;
    }

    ///
    /// \brief Adds the triangle _tri to the front tree, constructing the front tree if needed.
    ///        The add is recursive.
    ///
    /// \note This ignores the triangle if its area is too small, to avoid adding very thin strips
    ///
    /// \param _tri - Triangle to add
    ///
    void TriBSPTree::AddTriToFront(const Tri& _tri)
    {
        if ( _tri.Area() > 1e-3f )
        {
            if ( !m_frontTree )
            {
                m_frontTree = new TriBSPTree();
            }
            m_frontTree->AddTriangle(_tri);
        }
    }

    ///
    /// \brief Adds the triangle _tri to the back tree, constructing the back tree if needed.
    ///        The add is recursive.
    ///
    /// \note This ignores the triangle if its area is too small, to avoid adding very thin strips
    ///
    /// \param _tri - Triangle to add
    ///
    void TriBSPTree::AddTriToBack(const Tri& _tri)
    {
        if ( _tri.Area() > 1e-3f )
        {
            if ( !m_backTree )
            {
                m_backTree = new TriBSPTree();
            }
            m_backTree->AddTriangle(_tri);
        }
    }

    ///
    /// \brief Calculates a value f(_point) for the 3D point _point such that f(_point) < 0
    ///        if _point is behind _node's plane and f(_point) > 0 if it's in the front.
    ///
    ///        The implicit function here is actually just the signed distance of _point from the
    ///        _node's plane.
    ///
    /// \param _point         - Point at which to evaluate implicit function
    /// \param _node          - Node whose plane is used for the evaluating the function
    /// \param _snapToZeroTol - Abs tolerance. f's value is snapped to zero if below this.
    ///
    /// \return f's value for _point
    ///
    float TriBSPTree::CalcImplicitFunc(const glm::vec3& _point,
                                       const Plane* _plane,
                                       float _snapToZeroTol)
    {
        ASSERT(_plane, "Node triangle needs to have been populated to calc implicit func");
        return _plane->CalcSignedDist(_point, _snapToZeroTol);
    }

    ///
    /// \brief Finds the intersection point of a line segment with a triangle plane
    ///
    /// \param _start - Segment start
    /// \param _end   - Segment end
    /// \param _plane - Plane to intersect with
    /// \return
    ///
    glm::vec3 TriBSPTree::FindIntersectionWithTriPlane(const glm::vec3& _start,
                                                       const glm::vec3& _end,
                                                       const Plane* _plane)
    {
        float distStart = _plane->CalcSignedDist(_start, 0.0f);
        float distEnd = _plane->CalcSignedDist(_end, 0.0f);
        float t = distStart / (distStart - distEnd);
        return _start + t * (_end - _start);
    }
}
