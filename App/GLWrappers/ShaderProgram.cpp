#include "ShaderProgram.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "BlitheAssert.h"

namespace blithe
{
    /*!
     * \brief Constructor. Compiles the vertex shader at _vertexPath and fragment shader at
     *        _fragmentPath, binds some reserved attribute names to their reserved
     *        locations, then links the shaders to a program.
     *
     * \param _vertexPath   - Path to vertex shader
     * \param _fragmentPath - Path to fragment shader
     */
    ShaderProgram::ShaderProgram(const std::string& _vertexPath, const std::string& _fragmentPath)
    {
        tl::optional<std::string> vertexSrc = LoadShaderSource(_vertexPath);
        ASSERT(vertexSrc.has_value(), "Error opening vertex shader file: " << _vertexPath);
        tl::optional<std::string> fragmentSrc = LoadShaderSource(_fragmentPath);
        ASSERT(fragmentSrc.has_value(), "Error opening fragment shader file: " << _fragmentPath);

        GLuint vertexShader = CompileShader(vertexSrc.value(), GL_VERTEX_SHADER);
        GLuint fragmentShader = CompileShader(fragmentSrc.value(), GL_FRAGMENT_SHADER);

        m_programID = glCreateProgram();

        BindCommonAttribLocations();

        glAttachShader(m_programID, vertexShader);
        glAttachShader(m_programID, fragmentShader);
        glLinkProgram(m_programID);

        GLint linkStatus = CheckLinkingErrors();
        ASSERT(linkStatus, "Error linking shader program. Vertex shader:\n" << vertexSrc.value() << "\n\nFragment shader:\n" << fragmentSrc.value());

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    /*!
     * \brief Deletes the shader program.
     */
    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(m_programID);
    }

    /*!
     * \brief Binds the shader program.
     */
    void ShaderProgram::Bind() const
    {
        glUseProgram(m_programID);
    }

    /*!
     * \brief Unbinds the shader program.
     */
    void ShaderProgram::Unbind() const
    {
        glUseProgram(0);
    }

    /*!
     * \brief Sets the value _value for the floating-point uniform named _name.
     *
     * \param _name  - Name of the uniform, to look up its location
     * \param _value - Value to pass to the uniform
     */
    void ShaderProgram::SetUniform1f(const std::string& _name, float _value)
    {
        glUniform1f(glGetUniformLocation(m_programID, _name.c_str()), _value);
    }

    /*!
    * \brief Sets the value _value for the int uniform named _name.
    *
    * \param _name  - Name of the uniform, to look up its location
    * \param _value - Value to pass to the uniform
    */
    void ShaderProgram::SetUniform1i(const std::string& _name, int _value)
    {
        glUniform1i(glGetUniformLocation(m_programID, _name.c_str()), _value);
    }

    /*!
    * \brief Sets the value _value for the 3d floating-point uniform named _name.
    *
    * \param _name  - Name of the uniform, to look up its location
    * \param _value - Value to pass to the uniform
    */
    void ShaderProgram::SetUniformVec3f(const std::string& _name, const glm::vec3& _value)
    {
        glUniform3fv(glGetUniformLocation(m_programID, _name.c_str()), 1, &_value[0]);
    }

    /*!
    * \brief Sets the value _value for the 4x4 float matrix uniform named _name.
    *
    * \param _name  - Name of the uniform, to look up its location
    * \param _value - Value to pass to the uniform
    */
    void ShaderProgram::SetUniformMat4f(const std::string& _name, const glm::mat4& _value)
    {
        glUniformMatrix4fv(glGetUniformLocation(m_programID, _name.c_str()), 1, GL_FALSE, &_value[0][0]);
    }

    /*!
    * \brief Reads the string contents of the shader at _filePath.
    *
    * \param _filePath - Path to shader
    *
    * \return optional string. Only has a value if the shader source could be read from
    *         _filePath.
    */
    tl::optional<std::string> ShaderProgram::LoadShaderSource(const std::string& _filePath)
    {
        // Init empty optional result
        tl::optional<std::string> result;

        // Try to open the file
        std::ifstream file;
        file.open(_filePath.c_str());

        // If we could open file, read all its contents as a string.
        if (file.is_open())
        {
            std::stringstream stream;
            stream << file.rdbuf();
            file.close();

            result = stream.str();
        }

        return result;
    }

    /*!
     * \brief Compiles the shader with source code _source and of type _shaderType.
     *
     * \param _source     - GLSL source code of the shader
     * \param _shaderType - Type of shader (Vertex, Fragment, etc.)
     *
     * \return ID of the compiled shader
     */
    GLuint ShaderProgram::CompileShader(const std::string& _source, GLenum _shaderType)
    {
        GLuint shaderID = glCreateShader(_shaderType);
        const char* src = _source.c_str();
        glShaderSource(shaderID, 1, &src, nullptr);
        glCompileShader(shaderID);

        GLint result;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
        if (!result)
        {
            char infoLog[512];
            glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
            std::cerr << "Error compiling shader: " << infoLog << std::endl;
        }

        return shaderID;
    }

    /*!
     * \brief Checks the link status of the created shader program.
     *
     * \return Link status of created shader program
     */
    GLint ShaderProgram::CheckLinkingErrors()
    {
        GLint result;
        glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
        if (!result)
        {
            char infoLog[512];
            glGetProgramInfoLog(m_programID, 512, nullptr, infoLog);
            std::cerr << "Error linking program: " << infoLog << std::endl;
        }

        return result;
    }

    /*!
     * \brief Reserves attrib locations for commonly expected attrib names.
     */
    void ShaderProgram::BindCommonAttribLocations()
    {
        // Bind certain attribute locations
        glBindAttribLocation(m_programID, 0, "VertexPosition");
        glBindAttribLocation(m_programID, 1, "VertexColor");
        glBindAttribLocation(m_programID, 2, "VertexTexCoords");
    }
}