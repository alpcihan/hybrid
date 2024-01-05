#pragma once

#include "hybrid/renderer/camera.hpp"

namespace hybrid {

class CameraController {
public: 
    float speed = 3;
    float turnSpeed = 90;

public:
    CameraController(Camera& camera);
    void update(float deltaTime);

private:
    Camera& m_camera;

    float m_yaw;
    float m_pitch;
    glm::vec3 m_position;
};

}  // namespace hybrid