#include "GeomHelpers.h"
#include "Mesh.h"

namespace blithe
{
    ///
    /// \brief Calculates the centroid of the _mesh vertex positions.
    ///        If the _mesh has no vertices this just returns (0, 0, 0).
    ///
    /// \param _mesh - Mesh whose vertex position centroid is desired
    ///
    /// \return Centroid of the _mesh vertex positions.
    ///
    glm::vec3 GeomHelpers::CalcCentroid(const Mesh& _mesh)
    {
        glm::vec3 centroid(0, 0, 0);

        // Do a rolling average: new_centroid = (i*old_centroid + v[i]) / (i+1)
        for ( size_t i = 0; i < _mesh.m_vertices.size(); i++ )
        {
            const auto& v = _mesh.m_vertices[i].m_pos;
            centroid = centroid + (glm::vec3(v[0], v[1], v[2]) - centroid) / static_cast<float>(i + 1);
        }

        return centroid;
    }

    ///
    //// \brief Computes the local-space AABB of a mesh based on its untransformed vertices
    ///
    /// \param _mesh - The mesh whose local AABB is to be computed
    ///
    /// \return AABB enclosing all vertices in the mesh's local coordinate system
    ///
    AABB GeomHelpers::CalcLocalAABB(const Mesh& _mesh)
    {
        AABB aabb;

        if ( _mesh.m_vertices.empty() )
        {
            aabb.m_min = glm::vec3(0);
            aabb.m_max = glm::vec3(0);
        }
        else
        {
            aabb.m_min = _mesh.m_vertices[0].m_pos;
            aabb.m_max = _mesh.m_vertices[0].m_pos;
            for ( size_t i = 0; i < _mesh.m_vertices.size(); i++ )
            {
                const auto& p = _mesh.m_vertices[i].m_pos;
                aabb.m_min = glm::min(aabb.m_min, p);
                aabb.m_max = glm::max(aabb.m_max, p);
            }
        }

        return aabb;
    }

    ///
    //// \brief Computes the world-space AABB of a mesh by transforming each vertex by the model matrix.
    ///
    /// \param _mesh     - The mesh whose world-space AABB is to be computed
    /// \param _modelMat - The model matrix used to transform local vertices to world space
    ///
    /// \return AABB enclosing the transformed vertices in world space
    ///
    AABB GeomHelpers::CalcWorldAABB(const Mesh& _mesh, const glm::mat4& _modelMat)
    {
        AABB aabb;

        if ( _mesh.m_vertices.empty() )
        {
            aabb.m_min = glm::vec3(0);
            aabb.m_max = glm::vec3(0);
        }
        else
        {
            aabb.m_min = _modelMat * glm::vec4(_mesh.m_vertices[0].m_pos, 1.0);
            aabb.m_max = _modelMat * glm::vec4(_mesh.m_vertices[0].m_pos, 1.0);
            for ( size_t i = 0; i < _mesh.m_vertices.size(); i++ )
            {
                glm::vec3 p = _modelMat * glm::vec4(_mesh.m_vertices[i].m_pos, 1.0);
                aabb.m_min = glm::min(aabb.m_min, p);
                aabb.m_max = glm::max(aabb.m_max, p);
            }
        }

        return aabb;
    }

    ///
    //// \brief Transforms an AABB by applying a matrix to all 8 corners and recomputing the
    ///         bounding box.
    ///
    ///         This can be used to somewhat inexactly but quickly update the AABB for a mesh. It is
    ///         inexact because the AABB enclosing the generally non-world-aligned local AABB will
    ///         possibly not be the tightest bound, but it avoids needing to visit and transform
    ///         every vertex.
    ///
    /// \param _aabb - The original AABB to transform
    /// \param _mat  - The transformation matrix to apply
    ///
    /// \return New AABB that tightly fits the transformed corners
    ///
    AABB GeomHelpers::TransformAABB(const AABB& _aabb, const glm::mat4& _mat)
    {
        AABB result;

        glm::vec3 corners[8] =
        {
            glm::vec3(_aabb.m_min.x, _aabb.m_min.y, _aabb.m_min.z),
            glm::vec3(_aabb.m_max.x, _aabb.m_min.y, _aabb.m_min.z),
            glm::vec3(_aabb.m_min.x, _aabb.m_max.y, _aabb.m_min.z),
            glm::vec3(_aabb.m_max.x, _aabb.m_max.y, _aabb.m_min.z),
            glm::vec3(_aabb.m_min.x, _aabb.m_min.y, _aabb.m_max.z),
            glm::vec3(_aabb.m_max.x, _aabb.m_min.y, _aabb.m_max.z),
            glm::vec3(_aabb.m_min.x, _aabb.m_max.y, _aabb.m_max.z),
            glm::vec3(_aabb.m_max.x, _aabb.m_max.y, _aabb.m_max.z)
        };

        glm::vec3 transformedCorner = glm::vec3(_mat * glm::vec4(corners[0], 1.0f));
        result.m_min = transformedCorner;
        result.m_max = transformedCorner;

        for (int i = 1; i < 8; ++i)
        {
            transformedCorner = glm::vec3(_mat * glm::vec4(corners[i], 1.0f));
            result.m_min = glm::min(result.m_min, transformedCorner);
            result.m_max = glm::max(result.m_max, transformedCorner);
        }

        return result;
    }

    ///
    /// \brief Creates a cuboid mesh with _sides expressing the width x height x depth, and 8
    ///        _colors for each of the vertices.
    ///
    ///        We start with the front face of the cube. For each face, we consider vertices
    ///        in CCW fashion, starting with the bottom-left. So we create 2 triangles as
    ///        so:
    ///        1. Bottom Left, Bottom Right, Top Right
    ///        2. Bottom Left, Top Right, Top Left
    ///
    /// \param _sides - Width, Height and Depth of the cuboid.
    /// \param _colors - Color for each corner of the cuboid. Must have 8 elements.
    ///
    /// \return Mesh for the cuboid
    ///
    Mesh GeomHelpers::CreateCuboid(glm::vec3 _sides, const std::vector<glm::vec4>& _colors,
                                   const glm::mat4& _modelTransform/*=glm::mat4(1.0f)*/)
    {
        auto w = _sides.x, h = _sides.y, d =_sides.z;

        // Define the vertices of the unit cube
        std::vector<Vertex> vertices = {
            // Front face
            { { -1.0f, -1.0f,  1.0f },_colors[0], { 0.0f, 0.0f } }, // Bottom-left
            { { 1.0f, -1.0f,  1.0f }, _colors[1],{ 1.0f, 0.0f } }, // Bottom-right
            { { 1.0f,  1.0f,  1.0f }, _colors[2],{ 1.0f, 1.0f } }, // Top-right
            { { -1.0f,  1.0f,  1.0f },_colors[3],{ 0.0f, 1.0f } }, // Top-left

            // Back face
            { { -1.0f, -1.0f, -1.0f },_colors[5],{ 1.0f, 0.0f } }, // Bottom-left
            { { 1.0f, -1.0f, -1.0f }, _colors[4],{ 0.0f, 0.0f } }, // Bottom-right
            { { 1.0f,  1.0f, -1.0f }, _colors[7],{ 0.0f, 1.0f } }, // Top-right
            { { -1.0f,  1.0f, -1.0f },_colors[6],{ 1.0f, 1.0f } }, // Top-left

            // Left face
            { { -1.0f, -1.0f, -1.0f },_colors[4],{ 0.0f, 0.0f } }, // Bottom-left
            { { -1.0f, -1.0f,  1.0f },_colors[0],{ 1.0f, 0.0f } }, // Bottom-right
            { { -1.0f,  1.0f,  1.0f },_colors[3],{ 1.0f, 1.0f } }, // Top-right
            { { -1.0f,  1.0f, -1.0f },_colors[7],{ 0.0f, 1.0f } }, // Top-left

            // Right face
            { { 1.0f, -1.0f,  1.0f },_colors[1],{ 0.0f, 0.0f } }, // Bottom-left
            { { 1.0f, -1.0f, -1.0f },_colors[5],{ 1.0f, 0.0f } }, // Bottom-right
            { { 1.0f,  1.0f, -1.0f },_colors[6],{ 1.0f, 1.0f } }, // Top-right
            { { 1.0f,  1.0f,  1.0f },_colors[2],{ 0.0f, 1.0f } }, // Top-left

            // Top face
            { { -1.0f,  1.0f,  1.0f },_colors[3],{ 0.0f, 0.0f } }, // Bottom-left
            { { 1.0f,  1.0f,  1.0f }, _colors[2],{ 1.0f, 0.0f } }, // Bottom-right
            { { 1.0f,  1.0f, -1.0f }, _colors[6],{ 1.0f, 1.0f } }, // Top-right
            { { -1.0f,  1.0f, -1.0f },_colors[7],{ 0.0f, 1.0f } }, // Top-left

            // Bottom face
            { { -1.0f, -1.0f, -1.0f },_colors[4],{ 0.0f, 0.0f } }, // Bottom-left
            { { 1.0f, -1.0f, -1.0f }, _colors[5],{ 1.0f, 0.0f } }, // Bottom-right
            { { 1.0f, -1.0f,  1.0f }, _colors[1],{ 1.0f, 1.0f } }, // Top-right
            { { -1.0f, -1.0f,  1.0f },_colors[0],{ 0.0f, 1.0f } }, // Top-left
        };

        // Scale the vertex positions so we have the desired sides
        // Transform the vertex positions with the optional modelTransform (defaults to identity)
        for ( size_t i = 0; i < vertices.size(); i++)
        {
            vertices[i].m_pos *= glm::vec3{w/2, h/2, d/2};
            vertices[i].m_pos = _modelTransform * glm::vec4(vertices[i].m_pos, 1.0f);
        }

        // Define the indices for the cube (two triangles per face)
        std::vector<unsigned int> indices = {
            // Front face
            0, 1, 2, 0, 2, 3,
            // Back face
            4, 5, 6, 4, 6, 7,
            // Left face
            8, 9, 10, 8, 10, 11,
            // Right face
            12, 13, 14, 12, 14, 15,
            // Top face
            16, 17, 18, 16, 18, 19,
            // Bottom face
            20, 21, 22, 20, 22, 23
        };

        return Mesh{ vertices, indices };
    }
}
