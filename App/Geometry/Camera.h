#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace blithe
{
    class Camera
    {
    public:
        Camera();
        Camera(const glm::vec3& _position, const glm::vec3& _front, const glm::vec3& _worldUp);
        static Camera FromTarget(const glm::vec3& _position, const glm::vec3& _target,
                                 const glm::vec3& _worldUp);

        glm::mat4 GetViewMatrix() const;

        glm::vec3 GetPosition() const { return m_position; }
        void SetPosition(const glm::vec3& _position) { m_position = _position; }

        glm::vec3 GetFront() const { return m_front; }
        void SetFront(const glm::vec3& _front);
        void SetTarget(const glm::vec3& _target);

        glm::vec3 GetRight() const { return m_right; }

    private:
        void UpdateRightAndUp();

        glm::vec3 m_position;
        glm::vec3 m_front;
        glm::vec3 m_worldUp;
        glm::vec3 m_right;
        glm::vec3 m_up;
    };
}

#endif // CAMERA_H
