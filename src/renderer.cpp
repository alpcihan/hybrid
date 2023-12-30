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

    // init uniform data
    m_uniformDataStage = tgai.createStagingBuffer({sizeof(UniformData)});
    m_uniformData = static_cast<UniformData *>(tgai.getMapping(m_uniformDataStage));
    m_uniformBuffer = tgai.createBuffer({tga::BufferUsage::uniform, sizeof(UniformData), m_uniformDataStage});

    // init passes
    {
        // 0 - geometry pass
        {
            // shaders
            tga::Shader vs = tga::loadShader(HYBRID_SHADER_PATH("triangle_vert.spv"), tga::ShaderType::vertex, tgai);
            tga::Shader fs = tga::loadShader(HYBRID_SHADER_PATH("triangle_frag.spv"), tga::ShaderType::fragment, tgai);

            // input layout
            tga::InputLayout inputLayout({{{
                // S0
                tga::BindingType::uniformBuffer  // B0: uniform buffer
            }}});

            // pass
            m_geometryPass = tgai.createRenderPass(
                tga::RenderPassInfo{vs, fs}
                    .setClearOperations(tga::ClearOperation::all)
                    .setPerPixelOperations(tga::CompareOperation::less)
                    //.setRasterizerConfig({tga::FrontFace::counterclockwise, tga::CullMode::back})
                    .setInputLayout(inputLayout)
                    .setRenderTarget(m_gBuffer));

            // input sets
            m_geometryPassInputSets = {
                tgai.createInputSet({m_geometryPass,
                                     {
                                         {m_uniformBuffer, 0},
                                     },
                                     0}),
            };
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
            tga::Shader vs =
                tga::loadShader(HYBRID_SHADER_PATH("full_screen_triangle_vert.spv"), tga::ShaderType::vertex, tgai);
            tga::Shader fs = tga::loadShader(HYBRID_SHADER_PATH("lighting_frag.spv"), tga::ShaderType::fragment, tgai);

            // input layout
            tga::InputLayout inputLayout({
                {
                    // S0
                    tga::BindingType::uniformBuffer  // B0: uniform buffer
                },
                {
                    // S1
                    {tga::BindingType::sampler, 1},  // B0: gbuffer0
                    {tga::BindingType::sampler, 1},  // B1: gbuffer1
                    {tga::BindingType::sampler, 1}   // B2: gbuffer2
                },
            });

            // pass
            m_lightingPass = tgai.createRenderPass(
                tga::RenderPassInfo{vs, fs, window}.setInputLayout(inputLayout).setRenderTarget(m_window));

            // input sets
            m_lightingPassInputSets = {
                tgai.createInputSet({m_lightingPass,
                                     {
                                         {m_uniformBuffer, 0},
                                     },
                                     0}),
                tgai.createInputSet({m_lightingPass,
                                     {
                                         {m_gBuffer[0], 0},
                                         {m_gBuffer[1], 1},
                                         {m_gBuffer[2], 2},
                                     },
                                     1}),

            };
        }
    }
}

void Renderer::renderFrame(const Camera& camera) {
    auto& tgai = Application::get().getInterface();

    m_uniformData->projection = camera.getProjection();
    m_uniformData->view = camera.getView();

    auto nextFrame = tgai.nextFrame(m_window);
    m_cmd = tga::CommandRecorder{tgai, m_cmd}  // 0 - buffer update
                .bufferUpload(m_uniformDataStage, m_uniformBuffer, sizeof(UniformData))

                // 1 - geometry pass
                .setRenderPass(m_geometryPass, 0, {0, 0, 0, 1})
                .bindInputSet(m_geometryPassInputSets[0])
                .draw(3, 0)

                // 2 - custom geometry pass

                // 3 - lighting pass
                .setRenderPass(m_lightingPass, nextFrame)
                .bindInputSet(m_lightingPassInputSets[0])
                .bindInputSet(m_lightingPassInputSets[1])
                .draw(3, 0)

                // command end
                .endRecording();

    // execute commands and show the result
    tgai.execute(m_cmd);
    tgai.present(m_window, nextFrame);
}

}  // namespace hybrid