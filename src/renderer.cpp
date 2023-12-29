#include "hybrid/renderer.hpp"

#include "hybrid/application.hpp"

namespace hybrid {

Renderer::Renderer(tga::Window& window) : m_window(window), m_cmd() {
    auto& tgai = Application::get().getInterface();
    const auto [resX, resY] = Application::get().getScreenResolution();

    // init gbuffer
    m_gBuffer = {tgai.createTexture({resX, resY, tga::Format::r16g16b16a16_sfloat}),
                 tgai.createTexture({resX, resY, tga::Format::r16g16b16a16_sfloat}),
                 tgai.createTexture({resX, resY, tga::Format::r16g16b16a16_sfloat})};

    // init passes
    {
        // 0 - geometry pass
        {
            // shaders
            tga::Shader vs = tga::loadShader(HYBRID_SHADER_PATH("triangle_vert.spv"), tga::ShaderType::vertex, tgai);
            tga::Shader fs = tga::loadShader(HYBRID_SHADER_PATH("triangle_frag.spv"), tga::ShaderType::fragment, tgai);

            // input layout

            // pass
            m_geometryPass = tgai.createRenderPass(
                tga::RenderPassInfo{vs, fs}
                    .setClearOperations(tga::ClearOperation::all)
                    .setPerPixelOperations(tga::CompareOperation::less)
                    //.setRasterizerConfig({tga::FrontFace::counterclockwise, tga::CullMode::back})
                    .setRenderTarget(m_gBuffer));
            
            // input sets

        }

        // 1 - custom geometry pass
        {
            // shaders

            // input layout

            // pass

            // input sets
        }

        // 2 - lighting pass
        {
            // shader
            tga::Shader vs = tga::loadShader(HYBRID_SHADER_PATH("full_screen_triangle_vert.spv"), tga::ShaderType::vertex, tgai);
            tga::Shader fs = tga::loadShader(HYBRID_SHADER_PATH("lighting_frag.spv"), tga::ShaderType::fragment, tgai);

            // input layout
            tga::InputLayout inputLayout({
                {{{tga::BindingType::sampler, 1}, {tga::BindingType::sampler, 1}, {tga::BindingType::sampler, 1}}},
            });

            // pass
            m_lightingPass = tgai.createRenderPass(tga::RenderPassInfo{vs, fs, window}.setInputLayout(inputLayout).setRenderTarget(m_window));

            // input sets
            m_lightingPassInputSets = {
                tgai.createInputSet({m_lightingPass, {{m_gBuffer[0], 0}, {m_gBuffer[1], 1}, {m_gBuffer[2], 2}, }, 0}) // s0: b0, b1, b2
            };
        }
    }
}

void Renderer::renderFrame() {
    auto& tgai = Application::get().getInterface();

    auto nextFrame = tgai.nextFrame(m_window);

    m_cmd = tga::CommandRecorder{tgai, m_cmd}
                // 1 - geometry pass
                .setRenderPass(m_geometryPass, 0, {0, 0, 0, 1})
                .draw(3, 0)
                
                // 2 - custom geometry pass

                // 3 - lighting pass
                .setRenderPass(m_lightingPass, nextFrame)
                .bindInputSet(m_lightingPassInputSets[0])
                .draw(3, 0)

                // command end
                .endRecording();

    // execute commands and show the result
    tgai.execute(m_cmd);
    tgai.present(m_window, nextFrame);
}

}  // namespace hybrid