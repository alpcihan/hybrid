#pragma once

#include "hybrid/hybrid_shared.hpp"
#include "hybrid/camera.hpp"

namespace hybrid {

class CameraController {
public: 
    float speed = 1;
    float turnSpeed = 45;

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