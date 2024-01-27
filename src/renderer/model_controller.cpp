#include "hybrid/renderer/model_controller.hpp"

#include "hybrid/core/application.hpp"
#include "hybrid/hybrid_shared.hpp"

namespace hybrid {

const glm::vec3 ModelController::WORLD_UP_VECTOR(0, 1, 0);
const glm::vec3 ModelController::WORLD_FORWARD_VECTOR(0, 0, -1);
const glm::vec3 ModelController::WORLD_RIGHT_VECTOR(-1, 0, 0);

ModelController::ModelController(GameObject& gameObject)
    : m_gameObject(gameObject), m_yaw(0), m_pitch(0), m_position(0, 0, 0.5f) {

        m_gameObject.m_model = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0.5f)) * glm::mat4(1) *
                           glm::rotate(glm::mat4(1), glm::radians(0.0f), glm::vec3(1.f, 0.f, 0.f)) *
                           glm::scale(glm::mat4(1), glm::vec3(0.0005f));
    }

void ModelController::update(float deltaTime) {
    auto& tgai = Application::get().getInterface();
    auto window = Application::get().getWindow();

    float moveSpeed = speed;

    if (tgai.keyDown(window, tga::Key::Left)) m_yaw += deltaTime * turnSpeed;
    if (tgai.keyDown(window, tga::Key::Right)) m_yaw -= deltaTime * turnSpeed;
    if (tgai.keyDown(window, tga::Key::Up)) m_pitch -= deltaTime * turnSpeed;
    if (tgai.keyDown(window, tga::Key::Down)) m_pitch += deltaTime * turnSpeed;

    m_pitch = std::clamp(m_pitch, -89.f, 89.f);

    auto rot = glm::mat3_cast(glm::quat(glm::vec3(-glm::radians(m_pitch), glm::radians(m_yaw), 0.f)));
    m_lookDir = rot * WORLD_FORWARD_VECTOR;
    auto r = rot * WORLD_RIGHT_VECTOR;

    if (tgai.keyDown(window, tga::Key::W)) m_position += m_lookDir * deltaTime * moveSpeed;
    if (tgai.keyDown(window, tga::Key::S)) m_position -= m_lookDir * deltaTime * moveSpeed;

    if (tgai.keyDown(window, tga::Key::A)) m_position += r * deltaTime * moveSpeed;
    if (tgai.keyDown(window, tga::Key::D)) m_position -= r * deltaTime * moveSpeed;

    if (tgai.keyDown(window, tga::Key::Shift_Left)) m_position += WORLD_UP_VECTOR * deltaTime * moveSpeed;
    if (tgai.keyDown(window, tga::Key::Space)) m_position -= WORLD_UP_VECTOR * deltaTime * moveSpeed;

    m_gameObject.m_model = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0.5f)) * glm::mat4(1) *
                           glm::rotate(glm::mat4(1), glm::radians(0.0f), glm::vec3(1.f, 0.f, 0.f)) *
                           glm::scale(glm::mat4(1), glm::vec3(0.0005f));
}

}  // namespace hybrid