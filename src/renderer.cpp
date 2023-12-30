#include "hybrid/renderer.hpp"

#include "hybrid/application.hpp"

namespace hybrid {

Renderer::Renderer(tga::Window& window) : m_window(window), m_cmd(), m_tgai(Application::get().getInterface()) {
    _init();
}

void Renderer::renderFrame(const Camera& camera) {
    // update uniform data
    m_uniformData->projection = camera.getProjection();
    m_uniformData->view = camera.getView();

    auto nextFrame = m_tgai.nextFrame(m_window);
    m_cmd = tga::CommandRecorder{m_tgai, m_cmd}  // 0 - buffer update
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
    m_tgai.execute(m_cmd);
    m_tgai.present(m_window, nextFrame);
}

void Renderer::_init() {
    _initBuffers();
    _initPasses();
}

void Renderer::_initBuffers() {
    const auto [resX, resY] = Application::get().getScreenResolution();

    // init gbuffer
    m_gBuffer = {m_tgai.createTexture({resX, resY, tga::Format::r16g16b16a16_sfloat}),
                 m_tgai.createTexture({resX, resY, tga::Format::r16g16b16a16_sfloat}),
                 m_tgai.createTexture({resX, resY, tga::Format::r16g16b16a16_sfloat})};

    // init uniform data
    m_uniformDataStage = m_tgai.createStagingBuffer({sizeof(UniformData)});
    m_uniformData = static_cast<UniformData *>(m_tgai.getMapping(m_uniformDataStage));
    m_uniformBuffer = m_tgai.createBuffer({tga::BufferUsage::uniform, sizeof(UniformData), m_uniformDataStage});
}

void Renderer::_initPasses() {
    // 0 - geometry pass
    {
        // shaders
        tga::Shader vs = tga::loadShader(HYBRID_SHADER_PATH("triangle_vert.spv"), tga::ShaderType::vertex, m_tgai);
        tga::Shader fs = tga::loadShader(HYBRID_SHADER_PATH("triangle_frag.spv"), tga::ShaderType::fragment, m_tgai);

        // input layout
        tga::InputLayout inputLayout({{{
            // S0
            tga::BindingType::uniformBuffer  // B0: uniform buffer
        }}});

        // pass
        m_geometryPass =
            m_tgai.createRenderPass(tga::RenderPassInfo{vs, fs}
                                        .setClearOperations(tga::ClearOperation::all)
                                        .setPerPixelOperations(tga::CompareOperation::less)
                                        .setRasterizerConfig({tga::FrontFace::counterclockwise, tga::CullMode::back})
                                        .setInputLayout(inputLayout)
                                        .setRenderTarget(m_gBuffer));

        // input sets
        m_geometryPassInputSets = {
            m_tgai.createInputSet({m_geometryPass,
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
            tga::loadShader(HYBRID_SHADER_PATH("full_screen_triangle_vert.spv"), tga::ShaderType::vertex, m_tgai);
        tga::Shader fs = tga::loadShader(HYBRID_SHADER_PATH("lighting_frag.spv"), tga::ShaderType::fragment, m_tgai);

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
        m_lightingPass = m_tgai.createRenderPass(
            tga::RenderPassInfo{vs, fs, m_window}.setInputLayout(inputLayout).setRenderTarget(m_window));

        // input sets
        m_lightingPassInputSets = {
            m_tgai.createInputSet({m_lightingPass,
                                   {
                                       {m_uniformBuffer, 0},
                                   },
                                   0}),
            m_tgai.createInputSet({m_lightingPass,
                                   {
                                       {m_gBuffer[0], 0},
                                       {m_gBuffer[1], 1},
                                       {m_gBuffer[2], 2},
                                   },
                                   1}),

        };
    }
}

}  // namespace hybrid