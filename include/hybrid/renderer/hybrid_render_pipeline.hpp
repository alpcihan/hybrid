#pragma once

#include "hybrid/hybrid_shared.hpp"
#include "hybrid/renderer/camera.hpp"
#include "hybrid/renderer/vertex.hpp"

namespace hybrid {

class HybridRenderPipeline {
public:
    HybridRenderPipeline(tga::Window& window);

    void render(const Camera& camera);

private:
    struct UniformData {
        alignas(16) glm::mat4 projection;
        alignas(16) glm::mat4 view;
        alignas(16) glm::vec4 zBufferParams;
        alignas(16) glm::vec4 projectionParams;
        float time;
        glm::mat4 model; //to change
    };

private:
    tga::Interface& m_tgai;

    tga::Window m_window;
    tga::CommandBuffer m_cmd;

    // gbuffer
    std::vector<tga::Texture> m_gBuffer;

    // uniform buffer
    UniformData* m_uniformData;
    tga::StagingBuffer m_uniformDataStage;
    tga::Buffer m_uniformBuffer;

    //scene data
    std::vector<hybrid::Vertex> m_vertexList;
    std::vector<uint32_t> m_indexList;
    tga::TextureBundle m_diffuseColorTex;

    //scene buffers TO CHANGE
    tga::StagingBuffer m_vertexBufferStage;
    tga::Buffer m_vertexBuffer;
    tga::StagingBuffer m_indexBufferStage;
    tga::Buffer m_indexBuffer;


    // passes
    tga::RenderPass m_geometryPass;
    tga::RenderPass m_customGeometryPass;
    tga::RenderPass m_lightingPass;

    // input sets
    std::vector<tga::InputSet> m_geometryPassInputSets;
    std::vector<tga::InputSet> m_customGeometryPassInputSets;
    std::vector<tga::InputSet> m_lightingPassInputSets;

private:
    void _init();
    void _loadSceneData(); //to change
    void _initBuffers();
    void _initPasses();
    void _updateUniformData(const Camera& camera);
};

}  // namespace hybrid