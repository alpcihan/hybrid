#pragma once

#include "hybrid/renderer/game_object.hpp"
#include "hybrid/core/application.hpp"

namespace hybrid {

tga::VertexLayout Vertex::layout() {
    return {sizeof(Vertex),
            {
                {offsetof(Vertex, position), tga::Format::r32g32b32_sfloat},
                {offsetof(Vertex, normal), tga::Format::r32g32b32_sfloat},
                {offsetof(Vertex, uv), tga::Format::r32g32_sfloat},
            }};
}

GameObject::GameObject(const std::string& objPath, const std::string& diffTexPath) {
    tga::Obj object = tga::loadObj(objPath);
    // convert tga::Vertex to hybrid::Vertex
    for (auto v : object.vertexBuffer) {
        m_vertexList.push_back(hybrid::Vertex(v.position, v.normal, v.uv));
    }
    m_indexList = object.indexBuffer;
    m_diffuseColorTex = tga::loadTexture(diffTexPath, tga::Format::r8g8b8a8_srgb, tga::SamplerMode::linear, Application::get().getInterface());
}

// void GameObject::setModelMatrix(const glm::mat4& model){
//     m_model = model;
// }

}  // namespace hybrid
