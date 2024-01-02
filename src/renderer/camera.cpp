#pragma once

#include "hybrid/renderer/camera.hpp"

namespace hybrid {

Camera::Camera(float aspectRatio, float nearPlane, float farPlane)
    : m_aspectRatio(aspectRatio), m_nearPlane(nearPlane), m_farPlane(farPlane) {}

}  // namespace hybrid