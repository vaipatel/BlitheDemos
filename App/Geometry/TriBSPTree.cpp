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
        delete m_backTree;
        delete m_frontTree;
    }

    ///
    /// \brief Inserts triangle _tri into this BSP Tree.
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
            return;
        }

        // Classify the 3 vertices of _tri w.r.t. this node's plane
        float fa = CalcImplicitFunc(_tri.m_v0.m_pos, MIN_F_VAL);
        float fb = CalcImplicitFunc(_tri.m_v1.m_pos, MIN_F_VAL);
        float fc = CalcImplicitFunc(_tri.m_v2.m_pos, MIN_F_VAL);

        // If the implicit function evaluates to 0 for all 3 vertices of _tri, we classify _tri
        // as being coplanar with this node's plane ..
        if ( fa == 0.0f && fb == 0.0f && fc == 0.0f )
        {
            m_coplanarTris.push_back(_tri);
            return;
        }
        // .. else if the implicit function evaluates to <= 0, we classify _tri as being behind
        // this node's plane ..
        else if ( fa <= 0.0f && fb <= 0.0f && fc <= 0.0f )
        {
            AddTriToBack(_tri);
            return;
        }
        // .. else we classify _tri as being in front of this node's plane ..
        else if ( fa >= 0.0f && fb >= 0.0f && fc >= 0.0f )
        {
            AddTriToFront(_tri);
            return;
        }
        // .. else _tri intersects the plane, so we need to split it.
        else
        {
            SplitResult splitTris = SplitTriangle(_tri, {fa,fb,fc}, m_coplanarTris.front());

            // std::cout << std::fixed << "t0 area = " << t0.Area() << ", t1 area = " << t1.Area() << ", t2 area = " << t2.Area() << std::endl;
            if ( splitTris.m_loneInFront )
            {
                AddTriToFront(splitTris.m_t0);
                AddTriToBack(splitTris.m_t1);
                AddTriToBack(splitTris.m_t2);
            }
            else
            {
                AddTriToBack(splitTris.m_t0);
                AddTriToFront(splitTris.m_t1);
                AddTriToFront(splitTris.m_t2);
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
    void TriBSPTree::Traverse(TriBSPTree* _root, const glm::vec3& _cameraPos,
                              std::vector<std::vector<Tri>>& _outTris)
    {
        if ( !_root ) return;

        // Classify _cameraPos w.r.t. _root
        float d = _root->CalcImplicitFunc(_cameraPos, MIN_F_VAL);

        // As is stated in "IMAGE GENERATION PHASE" of "On visible surface generation by a
        // priori tree structures":
        //   Specifically, we are interested in the side (positive or negative) of the node's
        //   polygon where the current viewing position is located. Let's call the two sides the
        //   "containing" side and the "other" side. The traversal for a back-to-front ordering
        //   is 1) the "other" side, 2) the node, and 3) the "containing" side.
        if ( d > 0 )
        {
            // _cameraPos is in front, so start with the back tree
            Traverse(_root->m_backTree, _cameraPos, _outTris);
            _outTris.push_back(_root->m_coplanarTris);
            Traverse(_root->m_frontTree, _cameraPos, _outTris);
        }
        else if ( d < 0 )
        {
            // _cameraPos is in the back, so start with the front tree
            Traverse(_root->m_frontTree, _cameraPos, _outTris);
            _outTris.push_back(_root->m_coplanarTris);
            Traverse(_root->m_backTree, _cameraPos, _outTris);
        }
        else
        {
            // unsure, start with the front tree
            Traverse(_root->m_frontTree, _cameraPos, _outTris);
            Traverse(_root->m_backTree, _cameraPos, _outTris);
        }
    }

    TriBSPTree::SplitResult TriBSPTree::SplitTriangle(const Tri& _tri,
                                                      const std::array<float, 3>& _fs,
                                                      const Tri& _splitter)
    {
        std::vector<size_t> frontVertIdxes;
        std::vector<size_t> backVertIdxes;
        for ( size_t i = 0; i < _fs.size(); i++ )
        {
            if ( _fs[i] <= 0.0f )
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

        return {t0, t1, t2, oneFront};
    }

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
    ///        if _point is behind this node's plane and f(_point) > 0 if it's in the front.
    ///
    ///        The implicit function here is actually just the signed distance of _point from the
    ///        triangle's plane.
    ///
    /// \param _point - Point at which to evaluate implicit function
    ///
    /// \return
    ///
    float TriBSPTree::CalcImplicitFunc(const glm::vec3& _point, float _snapToZeroTol)
    {
        ASSERT(!m_coplanarTris.empty(), "Node triangle needs to have been populated to calc implicit func");

        glm::vec3 normal;
        float d;
        m_coplanarTris.front().CalcNormalAndD(normal, d);

        float val = glm::dot(normal, _point) + d;

        // Snap to zero if less than tolerance
        if ( std::abs(val) < _snapToZeroTol )
        {
            val = 0.0f;
        }

        return val;
    }

    ///
    /// \brief Finds the intersection point of a line segment with a triangle plane
    ///
    /// \param _start
    /// \param _end
    /// \param _tri
    /// \return
    ///
    glm::vec3 TriBSPTree::FindIntersectionWithTriPlane(const glm::vec3& _start,
                                                       const glm::vec3& _end,
                                                       const Tri& _tri)
    {
        glm::vec3 normal;
        float d;
        _tri.CalcNormalAndD(normal, d);

        float distStart = glm::dot(normal, _start) + d;
        float distEnd = glm::dot(normal, _end) + d;
        float t = distStart / (distStart - distEnd);
        return _start + t * (_end - _start);
    }
}
