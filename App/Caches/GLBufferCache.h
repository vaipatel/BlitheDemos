#ifndef GLBUFFERCACHE_H
#define GLBUFFERCACHE_H

#include <unordered_map>
#include "IGLBufferCache.h"

namespace blithe
{
    ///
    /// \brief Cache of OpenGL buffers and arrays
    ///
    class GLBufferCache : public IGLBufferCache
    {
    public:
        GLBufferCache();
        ~GLBufferCache() override;

        GLuint GetVertexBuffer(const std::string& _key,
                               const void* _data,
                               size_t _size) override;

        GLuint GetIndexBuffer(const std::string& _key,
                              const std::vector<unsigned int>& _indices) override;

        GLuint GetVertexArray(const std::string& _key,
                              GLuint _vbo,
                              GLuint _ebo,
                              const VBOVertexFormat& _vboVertexformat) override;

        void RemoveVertexBuffer(const std::string& _key) override;

        void RemoveIndexBuffer(const std::string& _key) override;

        void RemoveVertexArray(const std::string& _key) override;

        void CleanUp() override;

    private:
        std::unordered_map<std::string, GLuint> m_vboCache; //!< Cache of name -> VBO handle
        std::unordered_map<std::string, GLuint> m_eboCache; //!< Cache of name -> EBO handle
        std::unordered_map<std::string, GLuint> m_vaoCache; //!< Cache of name -> VAO handle
    };
}

#endif // GLBUFFERCACHE_H
