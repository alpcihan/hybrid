#include "hybrid/core/application.hpp"
#include "hybrid/hybrid_shared.hpp"
#include "hybrid/renderer/camera_controller.hpp"

namespace hybrid {

CameraController::CameraController(Camera& camera, ModelController& modelController)
    : m_camera(camera), m_modelController(modelController) {}

void CameraController::update(float deltaTime) {
    glm::vec3 spaceshipPosition = m_modelController.getPosition();

    float distanceBehind = 2.f; // Adjust this value for the desired distance behind
    float heightAbove = 0.2f;  // Adjust this value for the desired height
    glm::vec3 targetCameraPosition = spaceshipPosition - m_modelController.getLookDir() * distanceBehind +
                               m_modelController.getUpDir() * heightAbove;

    // Smoothly interpolate between the current and target camera positions
    float smoothFactor = 0.99f;  // Adjust this value for the desired smoothness
    glm::vec3 currentCameraPosition = glm::vec3(m_camera.m_view[3]);
    glm::vec3 cameraPosition = glm::mix(currentCameraPosition, targetCameraPosition, smoothFactor);

    m_camera.m_view = glm::lookAt(cameraPosition, spaceshipPosition, m_modelController.getWorldUp());
}

}  // namespace hybrid