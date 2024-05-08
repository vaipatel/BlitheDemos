#ifndef YAWPITCHCAMERADECORATOR_H
#define YAWPITCHCAMERADECORATOR_H

#include "CameraDecorator.h"

namespace blithe
{
    class YawPitchCameraDecorator : public CameraDecorator
    {
    public:
        YawPitchCameraDecorator();
        ~YawPitchCameraDecorator() override;

        void ProcessKeyboard(enCameraMovement _movement, float _deltaTime) override;
        void ProcessMouseMove(float _xOffset, float _yOffset, float _deltaTime,
                              bool _constrainPitch = true) override;

        float m_yawDeg;
        float m_pitchDeg;
    };
}

#endif // YAWPITCHCAMERADECORATOR_H
