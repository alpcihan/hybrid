#pragma once

#include "hybrid/hybrid_shared.hpp"
#include "hybrid/camera.hpp"

namespace hybrid {

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(float fov, float aspectRatio, float nearPlane = 0.01, float farPlane = 1000);
};

}  // namespace hybrid