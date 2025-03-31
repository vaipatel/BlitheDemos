#ifndef RAYMESHPICKER_H
#define RAYMESHPICKER_H

#include <optional.hpp>
#include <glm/glm.hpp>
#include <stddef.h>
#include <vector>
#include "Mesh.h"

namespace blithe
{
    ///
    /// \brief Class for selecting mesh AABBs using ray picking based on screen-space mouse position
    ///
    class RayMeshPicker
    {
    public:
        ///
        /// \brief Represents the result of a successful pick
        ///
        struct Result
        {
            size_t m_idx;        //!< Index of the picked mesh in the input vector
            glm::vec3 m_entryPt; //!< Entry point on the AABB surface where the ray intersects
        };

        RayMeshPicker();
        RayMeshPicker(int _viewPortWidth, int _viewPortHeight, glm::mat4 _invViewProj);

        tl::optional<Result> PickSingleMeshAABB(glm::vec2 _mousePos,
                                                const std::vector<Mesh>& _meshes,
                                                const std::vector<glm::mat4>& _modelMats);

        int m_viewPortWidth = 0;   //!< Width of the viewport in pixels
        int m_viewPortHeight = 0;  //!< Height of the viewport in pixels
        glm::mat4 m_invViewProj = glm::mat4(1.0); //!< Inverse of the view-projection matrix
    };
}

#endif // RAYMESHPICKER_H
