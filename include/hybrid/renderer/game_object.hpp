#pragma once

#include "hybrid/hybrid_shared.hpp"
#include "hybrid/renderer/vertex.hpp"

namespace hybrid {

class GameObject {
public:
    GameObject(const std::string& objPath, const std::string& diffTexPath);

    std::vector<hybrid::Vertex>& getVertexList() { return m_vertexList; };
    std::vector<uint32_t>& getIndexList() { return m_indexList; };
    const tga::Texture getDiffuseTexture() const { return m_diffuseColorTex; };
    glm::mat4& getModelMatrix() { return m_model; };
    void setModelMatrix(const glm::mat4& model);


private:
    std::vector<hybrid::Vertex> m_vertexList;
    std::vector<uint32_t> m_indexList;
    tga::Texture m_diffuseColorTex;
    glm::mat4 m_model = glm::mat4(1);
};

}  // namespace hybrid
