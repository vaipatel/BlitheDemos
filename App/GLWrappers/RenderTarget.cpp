#include "RenderTarget.h"
#include "BlitheAssert.h"
#include "Texture.h"

namespace BLE
{
    /*!
     * \brief Constructor that creates a texture and a framebuffer that targets it.
     *
     * \param _width        - Texture width
     * \param _height       - Texture height
     * \param _format       - Texture format
     * \param _type         - Texture type
     * \param _filterParam  - Texture filter
     */
    RenderTarget::RenderTarget(int _width, int _height, GLenum _format, GLenum _type, TextureData::FilterParam _filterParam)
    {
        m_targetTexture = new Texture(_width, _height, _format, _type, _filterParam, nullptr);

        glGenFramebuffers(1, &m_targetFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_targetFBO);

        // Is this how we do depth? Or we pass in a texture with some "depth" format? I don't know.
        // But if we do we'll probably have to have GL_NONE attachment in the draw buffers below
		// This seems to work for the default viewport render target, but no idea how well it generalizes.
        GLuint renderBuffer;
        glGenRenderbuffers(1, &renderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

        // I've done the setup of the framebuffer texture as a loop, pretending like I
        // might have multiple color attachment textures in the future.
        constexpr size_t numTextures = 1;
        static_assert(numTextures == 1, "Only supporting 1 color attachment texture per RenderTarget atm");
        std::vector<GLenum> drawBuffers(numTextures);
        for (unsigned int idx = 0; idx < numTextures; ++idx)
        {
            drawBuffers[idx] = GL_COLOR_ATTACHMENT0 + idx;
            unsigned int texHandle = m_targetTexture->GetHandle();
            glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[idx], GL_TEXTURE_2D, texHandle, 0);
        }

        // Set the drawBuffers
        glDrawBuffers(static_cast<GLsizei>(numTextures), drawBuffers.data());

        // Always check that our framebuffer is ok
        bool frameBufferStatusOk = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
        ASSERT(frameBufferStatusOk, "Frame buffer setup failed");

        // Unbind here and give back to default FBO.
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /*!
     * \brief Destructor.
     */
    RenderTarget::~RenderTarget()
    {
        CleanUp();
    }

    /*!
     * \brief Binds the framebuffer
     */
    void RenderTarget::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_targetFBO);

        if (m_depthTestEnabled)
        {
            glEnable(GL_DEPTH_TEST);
        }

        if (m_clearMask & GL_COLOR_BUFFER_BIT)
        {
            GLclampf r = static_cast<GLclampf>(m_clearColor.r);
            GLclampf g = static_cast<GLclampf>(m_clearColor.g);
            GLclampf b = static_cast<GLclampf>(m_clearColor.b);
            GLclampf a = static_cast<GLclampf>(m_clearColor.a);
            glClearColor(r, g, b, a);
        }

        if (m_clearMask != 0)
        {
            glClear(m_clearMask);
        }
    }

    void RenderTarget::UnBind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /*!
     * \brief Cleans up the framebuffer and target texture.
     */
    void RenderTarget::CleanUp()
    {
        glDeleteFramebuffers(1, &m_targetFBO);
        m_targetFBO = 0;

        delete m_targetTexture;
    }
}