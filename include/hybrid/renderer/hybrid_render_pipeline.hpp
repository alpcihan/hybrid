#pragma once

#include "hybrid/hybrid_shared.hpp"
#include "hybrid/renderer/camera.hpp"

namespace hybrid {

class HybridRenderPipeline {
public:
    HybridRenderPipeline(tga::Window& window);

    void render(const Camera& camera);

private:
    struct UniformData {
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec4 zBufferParams;
        glm::vec4 projectionParams;
        float time;
    };

private:
    tga::Interface& m_tgai;

    tga::Window m_window;
    tga::CommandBuffer m_cmd;

    // scene maps
    std::vector<tga::Texture> m_gBuffer;
    tga::Buffer m_shadowMap;
    tga::Texture m_specularReflectionTex;
    std::vector<tga::Texture> m_specularReflectionImgPyramid;

    // uniform buffer
    UniformData *m_uniformData;
    tga::StagingBuffer m_uniformDataStage;
    tga::Buffer m_uniformBuffer;

    // passes
    tga::RenderPass m_geometryPass;
    tga::RenderPass m_customGeometryPass;
    tga::ComputePass m_shadowPass;
    tga::ComputePass m_specularReflectionPass;
    tga::RenderPass m_lightingPass;

    // input sets
    std::vector<tga::InputSet> m_geometryPassInputSets;
    std::vector<tga::InputSet> m_customGeometryPassInputSets;
    std::vector<tga::InputSet> m_shadowInputSets;
    std::vector<tga::InputSet> m_specularReflectionPassInputSets;
    std::vector<tga::InputSet> m_lightingPassInputSets;

private:
    void _init();
    void _initResources();
    void _initPasses();
    void _updateUniformData(const Camera& camera);
};

}  // namespace hybrid