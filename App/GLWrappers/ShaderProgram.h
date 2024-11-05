#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "optional.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

namespace blithe
{
    /*!
    * \brief Handles the creation, binding/unbinding, uniform settings, destruction of shader programs.
    */
    class ShaderProgram
    {
    public:
        ShaderProgram(const std::string& _vertexPath, const std::string& _fragmentPath);
        ~ShaderProgram();

        void Bind() const;
        void Unbind() const;

        void SetUniformBool(const std::string& _name, bool _value);
        void SetUniform1f(const std::string& _name, float _value);
        void SetUniform1i(const std::string& _name, int _value);
        void SetUniformVec3f(const std::string& _name, const glm::vec3& _value);
        void SetUniformVec4f(const std::string& _name, const glm::vec4& _value);
        void SetUniformMat4f(const std::string& _name, const glm::mat4& _value);

    private:
        tl::optional<std::string> LoadShaderSource(const std::string& _filePath);
        GLuint CompileShader(const std::string& _source, GLenum _shaderType);
        GLint CheckLinkingErrors();
        void BindCommonAttribLocations();

        GLuint m_programID; //!< ID of the created shader program, obtained from OpenGL
    };
}

#endif // SHADERPROGRAM_H
