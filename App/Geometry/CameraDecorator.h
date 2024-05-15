#ifndef CAMERADECORATOR_H
#define CAMERADECORATOR_H

#include "Camera.h"

namespace blithe
{
    enum class enCameraMovement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    class CameraDecorator
    {
    public:
        virtual void ProcessKeyboard(enCameraMovement _movement, float _deltaTime) = 0;
        virtual void ProcessMouseMove(float _xOffset, float _yOffset, bool _leftDragged,
                                      float _deltaTime, bool _constrainPitch = true) = 0;
        virtual ~CameraDecorator() {}

        const Camera& GetCamera() const { return m_camera; }

        static constexpr float ACCEL = 2.5f;
        static constexpr float SENSITIVITY = 0.1f;

    protected:
        Camera m_camera;
        float m_accel = ACCEL;
        float m_sensitivity = SENSITIVITY;
    };
}

#endif // CAMERADECORATOR_H
