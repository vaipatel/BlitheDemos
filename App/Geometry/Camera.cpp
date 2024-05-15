#include "Camera.h"

namespace blithe
{
    Camera::Camera()
    {
        m_position = {0, 0, 1};
        m_front = {0, 0, -1};
        m_worldUp = {0, 1, 0};

        UpdateRightAndUp();
    }

    Camera::Camera(const glm::vec3& _position, const glm::vec3& _front, const glm::vec3& _worldUp)
    {
        m_position = _position;
        m_front = glm::normalize(_front);
        m_worldUp = glm::normalize(_worldUp);

        UpdateRightAndUp();
    }

    Camera Camera::FromTarget(const glm::vec3& _position, const glm::vec3& _target,
                              const glm::vec3& _worldUp)
    {
        return Camera(_position, _target - _position, _worldUp);
    }

    glm::mat4 Camera::GetViewMatrix() const
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

    void Camera::SetFront(const glm::vec3& _front)
    {
        m_front = glm::normalize(_front);
        UpdateRightAndUp();
    }

    void Camera::SetFrontAndRight(const glm::vec3& _front, const glm::vec3& _right)
    {
        glm::vec3 newUp = glm::normalize(glm::cross(_right, _front));
        m_front = _front;
        m_up = newUp;
        m_right = glm::normalize(glm::cross(m_front, m_up));
    }

    void Camera::SetTarget(const glm::vec3& _target)
    {
        SetFront(_target - m_position);
    }

    void Camera::UpdateRightAndUp()
    {
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }
}
