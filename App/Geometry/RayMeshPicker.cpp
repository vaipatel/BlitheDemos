#include "RayMeshPicker.h"
#include <algorithm>
#include "BlitheAssert.h"
#include "GeomHelpers.h"
#include "RayAABBIntersecter.h"

namespace blithe
{
    ///
    /// \brief Default constructor
    ///
    RayMeshPicker::RayMeshPicker()
    {
    }

    ///
    /// \brief Constructs the picker with viewport dimensions and inverse view-projection matrix
    ///
    /// \param _viewPortWidth  - Width of the viewport in pixels
    /// \param _viewPortHeight - Height of the viewport in pixels
    /// \param _invViewProj    - Inverse of the view-projection matrix
    ///
    RayMeshPicker::RayMeshPicker(int _viewPortWidth, int _viewPortHeight, glm::mat4 _invViewProj)
    {
        m_viewPortWidth = _viewPortWidth;
        m_viewPortHeight = _viewPortHeight;
        m_invViewProj = _invViewProj;
    }

    ///
    ///
    /// \brief Picks the closest mesh whose AABB intersects with the ray cast from mouse position
    ///
    /// \param _mousePos  - Mouse position in screen coordinates
    /// \param _meshes    - Vector of meshes to test
    /// \param _modelMats - Corresponding model matrices for each mesh
    ///
    /// \return Optional Result if a mesh was hit.
    ///
    tl::optional<RayMeshPicker::Result> RayMeshPicker::PickSingleMeshAABB(
            glm::vec2 _mousePos,
            const std::vector<Mesh>& _meshes,
            const std::vector<glm::mat4>& _modelMats)
    {
        ASSERT(m_viewPortWidth > 0 && m_viewPortHeight > 0, "RayMeshPicker not setup correctly");
        ASSERT(_meshes.size() == _modelMats.size(), "There should be one model matrix per mesh");

        tl::optional<Result> result;

        // Convert mousePos from [0,w]x[0,h] to [-1,1]x[-1,1], so we can unproject with invViewProj
        glm::vec2 mouseNDC;
        mouseNDC.x = (2.0f * _mousePos.x) / m_viewPortWidth - 1.0f;
        mouseNDC.y = 1.0f - (2.0f * _mousePos.y) / m_viewPortHeight;
        // std::cout << "Mouse NDC: " << mouseNDC.x << ", " << mouseNDC.y << std::endl;

        // Near and far points are at NDC z=-1 and z=1. Convert them to world space to get our ray.
        glm::vec4 rayStartNDC(mouseNDC, -1.0f, 1.0f);
        glm::vec4 rayEndNDC  (mouseNDC,  1.0f, 1.0f);
        glm::vec4 rayStartWorld = m_invViewProj * rayStartNDC;
        glm::vec4 rayEndWorld   = m_invViewProj * rayEndNDC;
        rayStartWorld /= rayStartWorld.w;
        rayEndWorld   /= rayEndWorld.w;
        glm::vec3 rayOrigin = glm::vec3(rayStartWorld);
        glm::vec3 rayDir = glm::normalize(glm::vec3(rayEndWorld - rayStartWorld));
        //std::cout << "Ray dir: " << rayDir.x << ", " << rayDir.y << ", " << rayDir.z << std::endl;

        std::vector<float> tnears;
        std::vector<RayIntersectionResult> intersectionResults;
        for ( size_t i = 0; i < _meshes.size(); i++ )
        {
            const Mesh& mesh = _meshes[i];
            AABB localAABB = GeomHelpers::CalcLocalAABB(mesh);
            AABB worldAABB = GeomHelpers::TransformAABB(localAABB, _modelMats[i]);
            tl::optional<RayIntersectionResult> intersection = RayAABBIntersecter::Intersect(
                                                                   rayOrigin,
                                                                   rayDir,
                                                                   worldAABB);
            if ( intersection.has_value() )
            {
                tnears.push_back(intersection->m_tClose);
                intersectionResults.push_back(intersection.value());
            }
        }

        if ( !tnears.empty() )
        {
            std::vector<float>::iterator minIt = std::min_element(tnears.begin(), tnears.end());
            Result r;
            r.m_idx = std::distance(tnears.begin(), minIt);
            r.m_entryPt = intersectionResults[r.m_idx].m_entryPt;
            result = r;
        }

        return result;
    }
}
