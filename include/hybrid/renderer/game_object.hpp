#pragma once

#include "hybrid/hybrid_shared.hpp"
#include "hybrid/renderer/vertex.hpp"

namespace hybrid {

class GameObject {
public:
    GameObject(std::vector<hybrid::Vertex> vertexList, std::vector<uint32_t> indexList, glm::mat4 model,
               tga::Texture diffuseColorTex);

private:
    std::vector<hybrid::Vertex> m_vertexList;
    std::vector<uint32_t> m_indexList;
    glm::mat4 m_model = glm::mat4(1);
    tga::Texture m_diffuseColorTex;
};

}  // namespace hybrid
