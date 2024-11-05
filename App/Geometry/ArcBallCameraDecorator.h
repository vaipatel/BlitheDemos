#ifndef ARCBALLCAMERADECORATOR_H
#define ARCBALLCAMERADECORATOR_H

#include "CameraDecorator.h"

namespace blithe
{
    class ArcBallCameraDecorator : public CameraDecorator
    {
    public:
        ArcBallCameraDecorator(const glm::vec3 _eye = {0.0f, 0.0f, 5.0f});
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
