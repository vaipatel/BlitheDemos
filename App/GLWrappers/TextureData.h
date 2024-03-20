#ifndef TEXTUREDATA_H
#define TEXTUREDATA_H

#include <glad/glad.h>

namespace BLE
{
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
}

#endif // TEXTUREDATA_H