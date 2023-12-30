#pragma once

#include "hybrid/hybrid_shared.hpp"
#include "hybrid/camera.hpp"

namespace hybrid {

class Renderer {
public:
    Renderer(tga::Window& window);

    void renderFrame(const Camera& camera);

private:
    struct UniformData {
        alignas(16) glm::mat4 projection;
        alignas(16) glm::mat4 view;
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
    void _initBuffers();
    void _initPasses();
};

}  // namespace hybrid