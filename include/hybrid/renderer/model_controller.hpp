#pragma once

#include "hybrid/renderer/game_object.hpp"

namespace hybrid {

class ModelController {
public:
    ModelController(GameObject& gameObject);
    void update(float deltaTime);
    const glm::vec3 getPosition() const { return m_position; };
    const glm::vec3 getUp() const { return WORLD_UP_VECTOR; };
    const glm::vec3 getForward() const { return WORLD_FORWARD_VECTOR; };
    const glm::vec3 getRight() const { return WORLD_RIGHT_VECTOR; };
    const glm::vec3 getLookDir() const { return m_lookDir; };

public:
    float speed = 3;
    float turnSpeed = 90;

private:
    GameObject& m_gameObject;

    float m_yaw;
    float m_pitch;
    glm::vec3 m_position;
    glm::vec3 m_lookDir;

    static const glm::vec3 WORLD_UP_VECTOR;
    static const glm::vec3 WORLD_FORWARD_VECTOR;
    static const glm::vec3 WORLD_RIGHT_VECTOR;
};

}  // namespace hybrid
