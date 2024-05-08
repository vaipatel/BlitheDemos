#include "YawPitchCameraDecorator.h"

namespace blithe
{
    YawPitchCameraDecorator::YawPitchCameraDecorator()
    {
        m_yawDeg = 0.0f;
        m_pitchDeg = 0.0f;
    }

    YawPitchCameraDecorator::~YawPitchCameraDecorator()
    {

    }

    void YawPitchCameraDecorator::ProcessKeyboard(enCameraMovement _movement, float _deltaTime)
    {
        float speed = m_accel * _deltaTime;
        switch (_movement)
        {
        case enCameraMovement::FORWARD:
            m_camera.SetPosition(m_camera.GetPosition() + m_camera.GetFront() * speed);
            break;
        case enCameraMovement::BACKWARD:
            m_camera.SetPosition(m_camera.GetPosition() - m_camera.GetFront() * speed);
            break;
        case enCameraMovement::RIGHT:
            m_camera.SetPosition(m_camera.GetPosition() + m_camera.GetRight() * speed);
            break;
        case enCameraMovement::LEFT:
            m_camera.SetPosition(m_camera.GetPosition() - m_camera.GetRight() * speed);
            break;
        }
    }

    void YawPitchCameraDecorator::ProcessMouseMove(float _xOffset, float _yOffset, float _deltaTime,
                                               bool _constrainPitch/*=true*/)
    {
        _CRT_UNUSED(_deltaTime);

        _xOffset *= m_sensitivity;
        _yOffset *= m_sensitivity;

        m_yawDeg   += _xOffset;
        m_pitchDeg += _yOffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if ( _constrainPitch )
        {
            if ( m_pitchDeg > 89.0f )
            {
                m_pitchDeg = 89.0f;
            }

            if ( m_pitchDeg < -89.0f )
            {
                m_pitchDeg = -89.0f;
            }
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(m_yawDeg)) * cos(glm::radians(m_pitchDeg));
        front.y = sin(glm::radians(m_pitchDeg));
        front.z = sin(glm::radians(m_yawDeg)) * cos(glm::radians(m_pitchDeg));
        m_camera.SetFront(front); // also updates right and up
    }
}
