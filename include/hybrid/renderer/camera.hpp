#pragma once

#include "hybrid/hybrid_shared.hpp"

namespace hybrid {

class Camera {
public:
    Camera(float aspectRatio, float nearPlane = 0.01, float farPlane = 1000);
    virtual ~Camera() = default;

    const glm::mat4& getProjection() const { return m_projection; }
    const glm::mat4& getView() const { return m_view; }
    float getAspectRatio() const { return m_aspectRatio; }
    float getNearPlane() const { return m_nearPlane; } 
    float getFarPlane() const { return m_farPlane; } 

protected:
    glm::mat4 m_projection = glm::mat4(1);
    glm::mat4 m_view = glm::mat4(1);
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;

friend class CameraController;
};

}  // namespace hybrid