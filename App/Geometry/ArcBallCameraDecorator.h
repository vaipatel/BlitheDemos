#ifndef ARCBALLCAMERADECORATOR_H
#define ARCBALLCAMERADECORATOR_H

#include "CameraDecorator.h"

namespace blithe
{
    class ArcBallCameraDecorator : public CameraDecorator
    {
    public:
        ArcBallCameraDecorator();
        ~ArcBallCameraDecorator() override;

        void ProcessKeyboard(enCameraMovement _movement, float _deltaTime) override;
        void ProcessMouseMove(float _xOffset, float _yOffset, bool _leftDragged,
                              float _deltaTime, bool _constrainPitch = true) override;

        glm::vec3 m_target;
        float m_distance;

    private:
        void UpdateCameraPosition();
    };
}

#endif // ARCBALLCAMERADECORATOR_H
