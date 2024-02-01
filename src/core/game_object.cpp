#include "hybrid/core/game_object.hpp"
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

GameObject::GameObject(const std::string& objPath, const std::string& diffTexPath, const std::string& specularTexPath, const std::string& roughnessTexPath) {
    tga::Obj object = tga::loadObj(objPath);
    // convert tga::Vertex to hybrid::Vertex
    for (auto v : object.vertexBuffer) {
        m_vertexList.push_back(hybrid::Vertex(v.position, v.normal, v.uv));
    }
    m_indexList = object.indexBuffer;
    m_diffuseColorTex = tga::loadTexture(diffTexPath, tga::Format::r8g8b8a8_srgb, tga::SamplerMode::linear, Application::get().getInterface());
    m_specularTex = tga::loadTexture(specularTexPath, tga::Format::r32_sfloat, tga::SamplerMode::linear, Application::get().getInterface());
    m_roughnessTex = tga::loadTexture(roughnessTexPath, tga::Format::r32_sfloat, tga::SamplerMode::linear, Application::get().getInterface());

}

const tga::ext::TransformMatrix GameObject::getExtTransform() { 
    
    return tga::ext::TransformMatrix({
        {
            {m_model[0].x,m_model[0].y,m_model[0].z,m_model[0].w},
            {m_model[1].x,m_model[1].y,m_model[1].z,m_model[1].w},
            {m_model[2].x,m_model[2].y,m_model[2].z,m_model[2].w},
        }
    }); 
}

}  // namespace hybrid
