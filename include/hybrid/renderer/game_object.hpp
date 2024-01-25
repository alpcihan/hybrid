#pragma once

#include "hybrid/hybrid_shared.hpp"

namespace hybrid {

struct Vertex {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 normal;
    alignas(16) glm::vec2 uv;

    static tga::VertexLayout layout();
};


class GameObject {
public:
    GameObject(const std::string& objPath, const std::string& diffTexPath);

    std::vector<hybrid::Vertex>& getVertexList() { return m_vertexList; };
    std::vector<uint32_t>& getIndexList() { return m_indexList; };
    const tga::Texture getDiffuseTexture() const { return m_diffuseColorTex; };
    const glm::mat4& getModelMatrix() const { return m_model; };
    const tga::ext::TransformMatrix getExtTransform();
    //void setModelMatrix(const glm::mat4& model);


private:
    std::vector<hybrid::Vertex> m_vertexList;
    std::vector<uint32_t> m_indexList;
    tga::Texture m_diffuseColorTex;
    glm::mat4 m_model = glm::mat4(1);

friend class ModelController;
};

}  // namespace hybrid
