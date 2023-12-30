#pragma once

#include "hybrid/perspective_camera.hpp"

namespace hybrid {

PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearPlane, float farPlane) {
    m_projection = glm::perspective_vk(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

}  // namespace hybrid