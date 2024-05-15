#include "ArcBallCameraDecorator.h"
#include <glm/gtx/quaternion.hpp>
#include <iostream>

namespace blithe
{
    ArcBallCameraDecorator::ArcBallCameraDecorator()
    {
        m_target = {0, 0, 0};
        m_distance = 5.0f;
        m_camera = Camera::FromTarget({0.0f, 0.0f, m_distance}, m_target, {0.0f, 1.0f, 0.0f});
        m_sensitivity = 1.0f;
    }

    ArcBallCameraDecorator::~ArcBallCameraDecorator()
    {

    }

    void ArcBallCameraDecorator::ProcessKeyboard(enCameraMovement _movement, float _deltaTime)
    {
        float speed = m_accel * _deltaTime;
        switch (_movement)
        {
        case enCameraMovement::FORWARD:
            m_distance -= speed;
            UpdateCameraPosition();
            break;
        case enCameraMovement::BACKWARD:
            m_distance += speed;
            UpdateCameraPosition();
            break;
        default:
            break;
        }
    }

    void ArcBallCameraDecorator::ProcessMouseMove(float _xOffset, float _yOffset, bool _leftDragged,
                                                  float _deltaTime, bool _constrainPitch/*=true*/)
    {
        _CRT_UNUSED(_deltaTime);
        _CRT_UNUSED(_constrainPitch);

        if (!_leftDragged)
            return;

        _xOffset *= m_sensitivity;
        _yOffset *= m_sensitivity;

        // Use x offset to calc a rot around up, and the y offset to calc a rot around right
        glm::quat qUp = glm::angleAxis(glm::radians(-_xOffset), m_camera.GetUp());
        glm::quat qRight = glm::angleAxis(glm::radians(_yOffset), m_camera.GetRight());

        // Compose the two rots to calc the overall angle and axis to rot front around
        glm::quat orientation = qRight * qUp;
        glm::vec3 newFront = glm::normalize(orientation * m_camera.GetFront());

        // Rotate right around up
        glm::vec3 newRight = glm::normalize(qUp * m_camera.GetRight());

        //glm::vec3 expectedNewUp = glm::normalize(qRight * m_camera.GetUp());

        // Update camera front and right (auto updates its up, then recalcs right to ensure
        // orthogonality)
        m_camera.SetFrontAndRight(newFront, newRight);

        //std::cout << abs(glm::dot(expectedNewUp, m_camera.GetUp())) << std::endl; // expect 1

        // Position camera
        UpdateCameraPosition();
    }

    ///
    /// \brief Wrapper to update the camera's position from the current target and distance,
    ///        and the camera's current front.
    ///
    void ArcBallCameraDecorator::UpdateCameraPosition()
    {
        m_camera.SetPosition(m_target - m_camera.GetFront() * m_distance);
    }
}

