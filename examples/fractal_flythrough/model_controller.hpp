#pragma once

#include "hybrid/hybrid.hpp"

class ModelController {
public:
    ModelController(std::shared_ptr<hybrid::GameObject> gameObject);
    void update(float deltaTime);
    const glm::vec3 getPosition() const { return m_position; };
    const glm::vec3 getWorldUp() const { return WORLD_UP_VECTOR; };
    const glm::vec3 getWorldForward() const { return WORLD_FORWARD_VECTOR; };
    const glm::vec3 getWorldRight() const { return WORLD_RIGHT_VECTOR; };
    const glm::vec3 getLookDir() const { return m_lookDir; };
    const glm::vec3 getUpDir() const { return m_upDir; };

public:
    float speed = 3;
    float turnSpeed = 90;

private:
    std::shared_ptr<hybrid::GameObject> m_gameObject;

    float m_yaw;
    float m_pitch;
    glm::vec3 m_position;
    glm::vec3 m_lookDir;
    glm::vec3 m_upDir;
    static const glm::vec3 WORLD_UP_VECTOR;
    static const glm::vec3 WORLD_FORWARD_VECTOR;
    static const glm::vec3 WORLD_RIGHT_VECTOR;
};
