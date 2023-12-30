#pragma once

#include "hybrid/hybrid_shared.hpp"

namespace hybrid {

class Camera {
public:
    Camera() = default;
    virtual ~Camera() = default;

    const glm::mat4& getProjection() const { return m_projection; }
    const glm::mat4& getView() const { return m_view; }

protected:
    glm::mat4 m_projection = glm::mat4(1);
    glm::mat4 m_view = glm::mat4(1);

friend class CameraController;
};

}  // namespace hybrid