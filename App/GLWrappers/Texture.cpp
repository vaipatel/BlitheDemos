#include "Texture.h"
#include "ImStrUtils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace IME
{
    /*!
     * \brief Constructs a texture with the given params and _data.
     *
     * \param _width        - Width of the texture
     * \param _height       - Height of the texture
     * \param _format       - Pixel format of the texture
     * \param _type         - Type of pixel data of the texture
     * \param _filterParam  - Filter to be used for the texture
     * \param _data         - Data for the texture. Could be nullptr, especially for FBO
     *                        applications.
     */
    Texture::Texture(int _width, int _height, GLenum _format, GLenum _type, TextureData::FilterParam _filterParam, const char *_data)
    {
        Construct(TextureData{ _width, _height, _format, _type, _filterParam, _data });
    }

    /*!
     * \brief Constructs an RGBA unsigned byte texture from the image at _imageFileName.
     *
     * \param _imageFileName - Path to image
     * \param _filterParam   - Filter to be used for the texture
     */
    Texture::Texture(const std::string& _imageFileName, TextureData::FilterParam _filterParam)
    {
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.. because OpenGL origin is bottom left while images usually start top left

        int width, height, nrChannels;
        unsigned char *data = stbi_load(_imageFileName.c_str(), &width, &height, &nrChannels, 4);

        bool couldLoad = data;
        if (data)
        {
            Construct(TextureData{ width, height, GL_RGBA, GL_UNSIGNED_BYTE, _filterParam, data });

            //glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }

        stbi_image_free(data);

        ASSERT(couldLoad, "Was unable to load image from " << _imageFileName);
    }

    /*!
     * \brief Destructor
     */
    Texture::~Texture()
    {
        CleanUp();
    }

    /*!
     * \brief Binds the texture at _text.
     *
     *        This first activates the texture unit at GL_TEXTURE0 + _activeUnitOffset.
     *
     * \param _activeUnitOffset - Offset relative to GL_TEXTURE0 of the active texture unit
     */
    void Texture::Bind(size_t _activeUnitOffset/* = 0*/) const
    {
        GLenum activeTextureUnitOffset = static_cast<GLenum>(_activeUnitOffset);
        glActiveTexture(GL_TEXTURE0 + activeTextureUnitOffset);

        glBindTexture(GL_TEXTURE_2D, GetHandle());
    }

    /*!
     * \brief Wrapper to clean up the texture.
     */
    void Texture::CleanUp()
    {
        glDeleteTextures(1, &m_handle);
        if (m_storedTextureData.m_data)
        {
            free(m_storedTextureData.m_data);
        }
    }

    /*!
     * \brief Wrapper used by constructors to construct a texture given _textureData.
     *
     * \param _textureData - The texture data to use
     */
    void Texture::Construct(const TextureData& _textureData)
    {
        ASSERT(_textureData.m_type != GL_NONE, "Texture type cannot be none.");
		ASSERT(_textureData.m_width > 0 && _textureData.m_height > 0, "Texture width/height cannot be 0. Got width " << _textureData.m_width << ", height " << _textureData.m_height);

        glGenTextures(1, &m_handle);

        StoredTextureData storedDataAtIdx;
        storedDataAtIdx.m_width = _textureData.m_width;
        storedDataAtIdx.m_height = _textureData.m_height;
        storedDataAtIdx.m_type = _textureData.m_type;
        storedDataAtIdx.m_filterParam = static_cast<GLint>(_textureData.m_filterParam);

        ASSERT(_textureData.m_format != GL_NONE, "Format for texture cannot be none.");
        storedDataAtIdx.m_format = _textureData.m_format;
        storedDataAtIdx.m_internalFormat = CalcInternalFormat(storedDataAtIdx.m_format, storedDataAtIdx.m_type);
        storedDataAtIdx.m_data = nullptr;

        // Copy the data if we have any
        if (_textureData.m_data)
        {
            // (If this image size calc is too unreliable maybe just pass in the image size.)
            size_t imageSize = static_cast<size_t>(storedDataAtIdx.m_width) *
                static_cast<size_t>(storedDataAtIdx.m_height) * CalcNumComponents(storedDataAtIdx.m_format) * CalcPerComponentSize(storedDataAtIdx.m_type);
            if (imageSize > 0)
            {
                storedDataAtIdx.m_data = malloc(imageSize);
                memcpy(storedDataAtIdx.m_data, _textureData.m_data, imageSize);
            }
        }

        m_storedTextureData = storedDataAtIdx;

        // (Do I HAVE to activate texture unit 0 before binding?)
        glActiveTexture(GL_TEXTURE0);

        // Bind the texture and specify it
        glBindTexture(GL_TEXTURE_2D, m_handle);
        glTexImage2D(GL_TEXTURE_2D,
            0,
            storedDataAtIdx.m_internalFormat,
            storedDataAtIdx.m_width,
            storedDataAtIdx.m_height,
            0,
            storedDataAtIdx.m_format,
            storedDataAtIdx.m_type,
            storedDataAtIdx.m_data);

        // Specify the filter and wrap
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, storedDataAtIdx.m_filterParam);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, storedDataAtIdx.m_filterParam);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Unbind
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /*!
     * \brief Calculates the format given the _internalFormat.
     *
     *        Based on https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml:
     *        Specifies the format of the pixel data. The following symbolic values are accepted (reordered and commented by me):
     *        GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL, GL_RED, GL_RG, GL_RGB, GL_RGBA, <- Base Internal Formats
     *        GL_BGR, GL_BGRA,
     *        GL_RED_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER, GL_BGR_INTEGER, GL_RGBA_INTEGER, GL_BGRA_INTEGER,
     *        GL_STENCIL_INDEX
     *
     * \param _internalFormat - Specified Internal Format
     *
     * \return Calculated format
     */
    GLenum Texture::CalcFormat(GLint _internalFormat)
    {
        GLenum format = GL_NONE;

        bool failed = false;
        switch (_internalFormat)
        {
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_STENCIL:
        case GL_RED:
        case GL_RG:
        case GL_RGB:
        case GL_RGBA:
            format = static_cast<GLenum>(_internalFormat);
            break;
        case GL_R8:
        case GL_R8_SNORM:
        case GL_R16:
        case GL_R16_SNORM:
        case GL_R16F:
        case GL_R32F:
        case GL_R8I:
        case GL_R8UI:
        case GL_R16I:
        case GL_R16UI:
        case GL_R32I:
        case GL_R32UI:
            format = GL_RED;
            break;
        case GL_RG8:
        case GL_RG8_SNORM:
        case GL_RG16:
        case GL_RG16_SNORM:
        case GL_RG16F:
        case GL_RG32F:
        case GL_RG8I:
        case GL_RG8UI:
        case GL_RG16I:
        case GL_RG16UI:
        case GL_RG32I:
        case GL_RG32UI:
            format = GL_RG;
            break;
        case GL_R3_G3_B2:
        case GL_RGB4:
        case GL_RGB5:
        case GL_RGB8:
        case GL_RGB8_SNORM:
        case GL_RGB10:
        case GL_RGB12:
        case GL_RGB16_SNORM:
        case GL_RGBA2:
        case GL_RGBA4:
        case GL_SRGB8:
        case GL_RGB16F:
        case GL_RGB32F:
        case GL_R11F_G11F_B10F:
        case GL_RGB9_E5:
        case GL_RGB8I:
        case GL_RGB8UI:
        case GL_RGB16I:
        case GL_RGB16UI:
        case GL_RGB32I:
        case GL_RGB32UI:
            format = GL_RGB;
            break;
        case GL_RGB5_A1:
        case GL_RGBA8:
        case GL_RGBA8_SNORM:
        case GL_RGB10_A2:
        case GL_RGB10_A2UI:
        case GL_RGBA12:
        case GL_RGBA16:
        case GL_SRGB8_ALPHA8:
        case GL_RGBA16F:
        case GL_RGBA32F:
        case GL_RGBA8I:
        case GL_RGBA8UI:
        case GL_RGBA16I:
        case GL_RGBA16UI:
        case GL_RGBA32I:
        case GL_RGBA32UI:
            format = GL_RGBA;
            break;
        default:
            failed = true;
        }

        ASSERT(!failed, "Could not deduce Texture for internal format " << IME::IntToHex(_internalFormat));

        return format;
    }

    /*!
     * \brief Calculates the number of components in each pixel, given the _format.
     *
     * \param _format - Specified format
     *
     * \return Number of components in each pixel, deduced from _format.
     */
    size_t Texture::CalcNumComponents(GLenum _format)
    {
        size_t numComponents = 0;

        bool failed = false;
        switch (_format)
        {
        case GL_DEPTH_COMPONENT:
        case GL_RED:
            numComponents = 1;
            break;
        case GL_DEPTH_STENCIL:
        case GL_RG:
            numComponents = 2;
            break;
        case GL_RGB:
        case GL_BGR:
            numComponents = 3;
            break;
        case GL_RGBA:
        case GL_BGRA:
            numComponents = 4;
            break;
        default:
            failed = true;
        }

        ASSERT(!failed, "Could not deduce Texture for internal format " << IME::IntToHex(_format));

        return numComponents;
    }

    /*!
     * \brief Calculates the data size of each pixel component based on the _type.
     *
     * \param _type - Specified Type
     *
     * \return Data size of each pixel component.
     */
    size_t Texture::CalcPerComponentSize(GLenum _type)
    {
        size_t componentSize = 0;

        bool failed = false;
        switch (_type)
        {
        case GL_UNSIGNED_BYTE:
        case GL_BYTE:
            componentSize = 1;
            break;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            componentSize = 2;
            break;
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_FLOAT:
            componentSize = 4;
            break;
        default:
            failed = true;
        }

        ASSERT(!failed, "Could not deduce Texture for internal format " << IME::IntToHex(_type));

        return componentSize;
    }

    /*!
     * \brief Calculates the Internal Format i.e. the number of color components of the
     *        texture, based on the _format and _type.
     *
     * \param _format - Specified Format
     * \param _type   - Specified Type
     *
     * \return Internal Format for the texture, deduced from the specified _format and _type.
     */
    int Texture::CalcInternalFormat(GLenum _format, GLenum _type) const
    {
        int internalFormat = GL_NONE;

        bool failed = false;
        switch (_type)
        {
        case GL_UNSIGNED_BYTE:
        case GL_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT:
        {
            switch (_format)
            {
            case GL_RED:
            case GL_RGB:
            case GL_RGBA:
                internalFormat = static_cast<GLint>(_format);
                break;
            default:
                failed = true;
                break;
            }
            break;
        }
        case GL_FLOAT:
        {
            switch (_format)
            {
            case GL_RED:
                internalFormat = GL_R32F;
                break;
            case GL_RGB:
                internalFormat = GL_RGB32F;
                break;
            case GL_RGBA:
                internalFormat = GL_RGBA32F;
                break;
            default:
                failed = true;
                break;
            }
            break;
        }
        default:
            failed = true;
            break;
        }

        ASSERT(!failed, "Could not deduce Texture for internal format " << IME::IntToHex(_format));

        return internalFormat;
    }
}