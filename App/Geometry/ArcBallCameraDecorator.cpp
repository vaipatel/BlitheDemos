#include "ArcBallCameraDecorator.h"
#include <glm/gtx/quaternion.hpp>
#include "BlitheShared.h"
//#include <iostream>

namespace blithe
{
    ArcBallCameraDecorator::ArcBallCameraDecorator(const glm::vec3 _eye/*= {0.0f, 0.0f, 5.0f}*/)
    {
        m_target = {0, 0, 0};
        m_distance = glm::distance(m_target, _eye);
        m_camera = Camera::FromTarget(_eye, m_target, {0.0f, 1.0f, 0.0f});
        m_sensitivity = 1.0f;

        UpdateCameraPosition();
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

    void ArcBallCameraDecorator::ProcessMouseMove(float _xOffset,
                                                  float _yOffset,
                                                  enCameraDraggedMouseBtn _draggedBtn,
                                                  float _deltaTime,
                                                  bool _constrainPitch/*=true*/)
    {
        UNUSED_ARG(_deltaTime);
        UNUSED_ARG(_constrainPitch);

        switch( _draggedBtn )
        {
            case enCameraDraggedMouseBtn::NONE:
            {
                break;
            }
            case enCameraDraggedMouseBtn::LEFT:
            {
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

                break;
            }
            case enCameraDraggedMouseBtn::RIGHT:
            {
                break;
            }
        }
    }

    void ArcBallCameraDecorator::ProcessMouseScroll(float _scrollX, float _scrollY, float _deltaTime)
    {
        m_distance += -_scrollY;
        //std::cout << "Scroll Y: " << _scrollY << std::endl;
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

