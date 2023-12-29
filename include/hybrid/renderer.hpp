#pragma once

#include "hybrid/hybrid_shared.hpp"

namespace hybrid {

class Renderer {
public:
    Renderer(tga::Window& window);

    void renderFrame();

private:
    tga::Window m_window;
    tga::CommandBuffer m_cmd;

    // gbuffer
    std::vector<tga::Texture> m_gBuffer;

    // passes
    tga::RenderPass m_geometryPass;
    tga::RenderPass m_customGeometryPass;
    tga::RenderPass m_lightingPass;

    // input sets
    std::vector<tga::InputSet> m_geometryPassInputSets;
    std::vector<tga::InputSet> m_customGeometryPassInputSets;
    std::vector<tga::InputSet> m_lightingPassInputSets;
};

}  // namespace hybrid