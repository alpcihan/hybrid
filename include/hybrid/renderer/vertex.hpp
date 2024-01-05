#pragma once

#include "hybrid/hybrid_shared.hpp"

namespace hybrid {

struct Vertex {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 normal;
    alignas(16) glm::vec2 uv;

    static tga::VertexLayout layout();
};

}  // namespace hybrid
