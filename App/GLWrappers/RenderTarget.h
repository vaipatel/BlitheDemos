#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "TextureData.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace IME
{
    class Texture;

    /*!
     * \brief Render target for rendering to framebuffers targetting textures.
     */
    class RenderTarget
    {
    public:
        RenderTarget(int _width, int _height, GLenum _format, GLenum _type, TextureData::FilterParam _filterParam);
        ~RenderTarget();

        void Bind();
        void CleanUp();

        unsigned int GetTargetFBO() const { return m_targetFBO; }
        Texture* GetTargetTexture() const { return m_targetTexture; }
        bool GetDepthTestEnabled() const { return m_depthTestEnabled; }
        void SetDepthTestEnabled(bool _enable) { m_depthTestEnabled = _enable; }
        glm::vec4 GetClearColor() const { return m_clearColor; }
        void SetClearColor(glm::vec4 _color) { m_clearColor = _color; }
        GLbitfield GetClearMask() const { return m_clearMask; }
        void SetClearMask(GLbitfield _mask) { m_clearMask = _mask; }

    private:
        Texture* m_targetTexture = nullptr; //!< Target texture
        unsigned int m_targetFBO = 0;       //!< ID of framebuffer, assigned by OpenGL
        bool m_depthTestEnabled = false;    //!< Whether depth testing is enabled
        glm::vec4 m_clearColor;             //!< The color to clear to when binding this
        GLbitfield m_clearMask = 0x0000;    //!< Mask specifying buffers we'd like to clear
    };
}

#endif // RENDERTARGET_H
