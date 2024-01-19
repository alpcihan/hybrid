#pragma once

#include "hybrid/renderer/camera.hpp"
#include "hybrid/renderer/model_controller.hpp"

namespace hybrid {

class CameraController {
public:
    CameraController(Camera& camera, ModelController& modelController);
    void update(float deltaTime);

private:
    Camera& m_camera;
    ModelController& m_modelController;
};

}  // namespace hybrid