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

const tga::ext::TransformMatrix GameObject::getExtTransform() { 
    
    return tga::ext::TransformMatrix({
        {m_model[0][0],m_model[0][1],m_model[0][2],m_model[0][3]},
        {m_model[1][0],m_model[1][1],m_model[1][2],m_model[1][3]},
        {m_model[2][0],m_model[2][1],m_model[2][2],m_model[2][3]},
    }); 
}

}  // namespace hybrid
