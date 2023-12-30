#pragma once

#include "hybrid/camera_controller.hpp"

#include "hybrid/application.hpp"

namespace hybrid {

static const glm::vec3 WORLD_UP_VECTOR(0, -1, 0);
static const glm::vec3 WORLD_FRONT_VECTOR(0, 0, -1);
static const glm::vec3 WORLD_RIGHT_VECTOR(1, 0, 0);

CameraController::CameraController(Camera& camera) : m_camera(camera), m_yaw(0), m_pitch(0), m_position(0) {}

void CameraController::update(float deltaTime) {
    auto& tgai = Application::get().getInterface();
    auto window = Application::get().getWindow();

    float moveSpeed = speed;

    if (tgai.keyDown(window, tga::Key::Left)) m_yaw -= deltaTime * turnSpeed;
    if (tgai.keyDown(window, tga::Key::Right)) m_yaw += deltaTime * turnSpeed;
    if (tgai.keyDown(window, tga::Key::Up)) m_pitch += deltaTime * turnSpeed;
    if (tgai.keyDown(window, tga::Key::Down)) m_pitch -= deltaTime * turnSpeed;

    m_pitch = std::clamp(m_pitch, -89.f, 89.f);

    auto rot = glm::mat3_cast(glm::quat(glm::vec3(-glm::radians(m_pitch), glm::radians(m_yaw), 0.f)));
    glm::vec3 lookDir = rot * WORLD_FRONT_VECTOR;
    auto r = rot * WORLD_RIGHT_VECTOR;

    if (tgai.keyDown(window, tga::Key::W)) m_position += lookDir * deltaTime * moveSpeed;
    if (tgai.keyDown(window, tga::Key::S)) m_position -= lookDir * deltaTime * moveSpeed;

    if (tgai.keyDown(window, tga::Key::A)) m_position += r * deltaTime * moveSpeed;
    if (tgai.keyDown(window, tga::Key::D)) m_position -= r * deltaTime * moveSpeed;

    if (tgai.keyDown(window, tga::Key::Shift_Left)) m_position += WORLD_UP_VECTOR * deltaTime * moveSpeed;
    if (tgai.keyDown(window, tga::Key::Space)) m_position -= WORLD_UP_VECTOR * deltaTime * moveSpeed;

    m_camera.m_view = glm::lookAt(m_position, m_position + lookDir, WORLD_UP_VECTOR);
}

}  // namespace hybrid