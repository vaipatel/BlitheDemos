#ifndef IGLBUFFERCACHE_H
#define IGLBUFFERCACHE_H

#include <string>
#include <vector>
#include "VBOVertexFormat.h"

namespace blithe
{
    ///
    /// \brief Interface to cache of OpenGL buffers and arrays.
    ///
    class IGLBufferCache
    {
    public:
        virtual ~IGLBufferCache() = default;

        virtual GLuint GetVertexBuffer(const std::string& _key,
                                       const void* _data,
                                       size_t _size) = 0;

        virtual GLuint GetIndexBuffer(const std::string& _key,
                                      const std::vector<unsigned int>& _indices) = 0;

        virtual GLuint GetVertexArray(const std::string& _key,
                                      GLuint _vbo,
                                      GLuint _ebo,
                                      const VBOVertexFormat& _vboVertexformat) = 0;

        virtual void RemoveVertexBuffer(const std::string& _key) = 0;

        virtual void RemoveIndexBuffer(const std::string& _key) = 0;

        virtual void RemoveVertexArray(const std::string& _key) = 0;

        virtual void CleanUp() = 0;
    };
}
#endif // IGLBUFFERCACHE_H
