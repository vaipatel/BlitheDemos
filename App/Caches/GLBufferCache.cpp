#include "GLBufferCache.h"
#include "BlitheAssert.h"

namespace blithe
{
    ///
    /// \brief Constructor
    ///
    GLBufferCache::GLBufferCache()
    {
    }

    ///
    /// \brief Destructor. At the moment, this cache expects the client to CleanUp() before
    ///        destructing. (Also, we shouldn't call the virtual CleanUp() in the destructor)
    ///
    GLBufferCache::~GLBufferCache()
    {
        bool allCleaned = m_vboCache.empty() && m_eboCache.empty() && m_vaoCache.empty();
        ASSERT(allCleaned, "Please call CleanUp() before destructing GLBufferCache");
    }

    ///
    /// \brief Gets the VBO handle for the _key. If the _key is not in the VBO cache, this uploads
    ///        the _data to the graphics card and obtains a handle, which it caches and returns.
    ///
    /// \param _key  - Name to use for lookup in the cache
    /// \param _data - Vertex data to be uploaded if not cached
    /// \param _size - Size of the vertex data to be uploaded
    ///
    /// \return VBO handle for the name _key
    ///
    GLuint GLBufferCache::GetVertexBuffer(const std::string& _key, const void* _data, size_t _size)
    {
        GLuint vbo;

        bool needsUpload = m_vboCache.find(_key) == m_vboCache.end();
        if ( needsUpload )
        {
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(_size), _data, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            m_vboCache[_key] = vbo;
        }
        vbo = m_vboCache[_key];

        return vbo;
    }

    ///
    /// \brief Gets the EBO handle for the _key. If the _key is not in the EBO cache, this uploads
    ///        the _indices to the graphics card and obtains a handle, which it caches and returns.
    ///
    /// \param _key     - Name to use for lookup in the cache
    /// \param _indices - Index data to be uploaded if not cached
    ///
    /// \return EBO handle for the name _key
    ///
    GLuint GLBufferCache::GetIndexBuffer(const std::string& _key,
                                         const std::vector<unsigned int>& _indices)
    {
        GLuint ebo;

        bool needsUpload = m_eboCache.find(_key) == m_eboCache.end();
        if ( needsUpload )
        {
            glGenBuffers(1, &ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         static_cast<GLsizeiptr>(_indices.size() * sizeof(unsigned int)),
                         _indices.data(),
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            m_eboCache[_key] = ebo;
        }
        ebo = m_eboCache[_key];

        return ebo;
    }

    ///
    /// \brief Gets the VAO handle for the _key. If the _key is not in the VAO cache, this uploads
    ///        the _vboVertexFormat to the graphics card and obtains a handle, which it caches and
    ///        returns.
    ///
    /// \param _key             - Name to use for lookup in the cache
    /// \param _vbo             - VBO to bind in case the format needs to be uploaded
    /// \param _ebo             - EBO to bind in case the format needs to be uploaded
    /// \param _vboVertexformat - Vertex format to be uploaded if not cached
    ///
    /// \return EBO handle for the name _key
    ///
    GLuint GLBufferCache::GetVertexArray(const std::string& _key,
                                         GLuint _vbo,
                                         GLuint _ebo,
                                         const VBOVertexFormat& _vboVertexformat)
    {
        GLuint vao;

        bool needsUpload = m_vaoCache.find(_key) == m_vaoCache.end();
        if ( needsUpload )
        {
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

            for (const auto& attr : _vboVertexformat.m_attributes)
            {
                glEnableVertexAttribArray(attr.m_index);
                glVertexAttribPointer(attr.m_index,
                                      attr.m_size,
                                      attr.m_type,
                                      attr.m_normalized,
                                      _vboVertexformat.m_stride,
                                      reinterpret_cast<void*>(attr.m_offset));
            }

            glBindVertexArray(0);

            m_vaoCache[_key] = vao;
        }
        vao = m_vaoCache[_key];

        return vao;
    }

    void GLBufferCache::RemoveVertexBuffer(const std::string& _key)
    {
        auto it = m_vboCache.find(_key);
        if ( it != m_vboCache.end() )
        {
            GLuint vbo = it->second;
            glDeleteBuffers(1, &vbo);
            m_vboCache.erase(it);
        }
    }

    void GLBufferCache::RemoveIndexBuffer(const std::string& _key)
    {
        auto it = m_eboCache.find(_key);
        if ( it != m_eboCache.end() )
        {
            GLuint ebo = it->second;
            glDeleteBuffers(1, &ebo);
            m_eboCache.erase(it);
        }
    }

    void GLBufferCache::RemoveVertexArray(const std::string& _key)
    {
        auto it = m_vaoCache.find(_key);
        if ( it != m_vaoCache.end() )
        {
            GLuint vao = it->second;
            glDeleteVertexArrays(1, &vao);
            m_vaoCache.erase(it);
        }
    }

    ///
    /// \brief Deletes all the buffers and arrays in the caches, and clears the caches.
    ///
    void GLBufferCache::CleanUp()
    {
        for (auto& pair : m_vboCache)
        {
            GLuint vbo = pair.second;
            glDeleteBuffers(1, &vbo);
        }
        m_vboCache.clear();

        for (auto& pair : m_eboCache)
        {
            GLuint ebo = pair.second;
            glDeleteBuffers(1, &ebo);
        }
        m_eboCache.clear();

        for (auto& pair : m_vaoCache)
        {
            GLuint vao = pair.second;
            glDeleteVertexArrays(1, &vao);
        }
        m_vaoCache.clear();
    }
}
