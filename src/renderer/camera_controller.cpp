#include "hybrid/renderer/camera_controller.hpp"

#include "hybrid/core/application.hpp"
#include "hybrid/hybrid_shared.hpp"

namespace hybrid {

CameraController::CameraController(Camera& camera, ModelController& modelController)
    : m_camera(camera), m_modelController(modelController) {}

void CameraController::update(float deltaTime) {
    auto& tgai = Application::get().getInterface();
    auto window = Application::get().getWindow();

    glm::vec3 spaceshipPosition = m_modelController.getPosition();

    float distanceBehind = 2.f;
    glm::vec3 cameraPosition = spaceshipPosition - m_modelController.getLookDir() * distanceBehind;

    m_camera.m_view = glm::lookAt(cameraPosition, spaceshipPosition, m_modelController.getUp());
}

}  // namespace hybrid