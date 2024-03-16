#ifndef TEXTURE_H
#define TEXTURE_H

#include "ImAssert.h"

#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

/*!
 * \brief Data needed for constructing a texture
 */
struct TextureData
{
    /*!
     * \brief Represents texture filter preference
     */
    enum class FilterParam
    {
        NEAREST = GL_NEAREST,  //!< Nearest interpolation
        LINEAR = GL_LINEAR     //!< Linear interpolation
    };

    int m_width;               //!< Texture width
    int m_height;              //!< Texture height
    GLenum m_format;           //!< The format of the pixel data
    GLenum m_type;             //!< The data type of the pixel data
    FilterParam m_filterParam; //!< Param to use for GL_TEXTURE_MAG_FILTER and GL_TEXTURE_MIN_FILTER
    const void* m_data;        //!< The texture data (can be null)
};

/*!
 * \brief Handles the creation, binding/unbinding, destruction of a GL texture.
 */
class Texture
{
public:
    /*!
     * \brief Stored texture attributes and data.
     * 
     *        The idea was TextureData will be "GL-free" and StoredTextureData can be
     *        more "GL-heavy". Not sure I've achieved that at all, or if it's worth it.
     */
    struct StoredTextureData
    {
        GLint m_internalFormat; //!< Number of color components
        int m_width;            //!< Texture width
        int m_height;           //!< Texture height
        GLenum m_format;        //!< The format of the pixel data
        GLenum m_type;          //!< The data type of the pixel data
        GLint m_filterParam;    //!< Param to use for GL_TEXTURE_MAG_FILTER and GL_TEXTURE_MIN_FILTER
        void* m_data;           //!< The texture data (can be null)
    };

    Texture(int _width, int _height, GLenum _format, GLenum _type, TextureData::FilterParam _filterParam, const char *_data);
    Texture(const std::string& _imageFileName, TextureData::FilterParam _filterParam);
    ~Texture();

    void Bind(size_t _activeUnitOffset = 0) const;
    void CleanUp();

    unsigned int GetHandle() const { return m_handle; }
    int GetWidth() const { return m_storedTextureData.m_width; }
    int GetHeight() const { return m_storedTextureData.m_height; }
    GLenum GetFormat() const { return m_storedTextureData.m_format; }
    GLenum GetType() const { return m_storedTextureData.m_type; }
    GLint GetInternalFormat() const { return m_storedTextureData.m_internalFormat; }

private:
    void Construct(const TextureData& _textureData);

    static GLenum CalcFormat(GLint _internalFormat);
    static size_t CalcNumComponents(GLenum _format);
    static size_t CalcPerComponentSize(GLenum _type);
    int CalcInternalFormat(GLenum _format, GLenum _type) const;

    unsigned int m_handle;                 //!< Handle to texture, assigned by OpenGL
    StoredTextureData m_storedTextureData; //!< The stored texture attributes and data
};

#endif // TEXTURE_H