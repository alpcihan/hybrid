#include "hybrid/renderer/hybrid_render_pipeline.hpp"

#include "hybrid/core/application.hpp"
#include "hybrid/core/time.hpp"
#include "hybrid/renderer/vertex.hpp"

namespace hybrid {

HybridRenderPipeline::HybridRenderPipeline(tga::Window& window, hybrid::GameObject& gameObject)
    : m_window(window), m_cmd(), m_tgai(Application::get().getInterface()), m_gameObject(gameObject) {
    _init();
}

void HybridRenderPipeline::render(const Camera& camera) {
    _updateUniformData(camera);

    auto nextFrame = m_tgai.nextFrame(m_window);
    m_cmd = tga::CommandRecorder{m_tgai, m_cmd}  // 0 - buffer update
                .bufferUpload(m_uniformDataStage, m_uniformBuffer, sizeof(UniformData))

                // 1 - geometry pass
                .setRenderPass(m_geometryPass, 0, {0, 0, 0, 1})
                .bindInputSet(m_geometryPassInputSets[0])
                .bindVertexBuffer(m_vertexBuffer)
                .bindIndexBuffer(m_indexBuffer)
                .drawIndexed(m_gameObject.getIndexList().size(), 0, 0)

                // 2 - custom geometry pass
                .setRenderPass(m_customGeometryPass, 0)
                .bindInputSet(m_customGeometryPassInputSets[0])
                .draw(3, 0)

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

void HybridRenderPipeline::_init() {
    _initBuffers();
    _initPasses();
}


void HybridRenderPipeline::_initBuffers() {
    const auto [resX, resY] = Application::get().getScreenResolution();

    // init gbuffer
    m_gBuffer = {m_tgai.createTexture({resX, resY, tga::Format::r16g16b16a16_sfloat}),
                 m_tgai.createTexture({resX, resY, tga::Format::r16g16b16a16_sfloat}),
                 m_tgai.createTexture({resX, resY, tga::Format::r16g16b16a16_sfloat})};

    // init uniform buffer
    m_uniformDataStage = m_tgai.createStagingBuffer({sizeof(UniformData)});
    m_uniformData = static_cast<UniformData *>(m_tgai.getMapping(m_uniformDataStage));
    m_uniformBuffer = m_tgai.createBuffer({tga::BufferUsage::uniform, sizeof(UniformData), m_uniformDataStage});

    // init scene buffers (vertexBuffer and indexBuffer)
    size_t vertexListSize = m_gameObject.getVertexList().size() * sizeof(hybrid::Vertex);
    m_vertexBufferStage = m_tgai.createStagingBuffer({vertexListSize, tga::memoryAccess(m_gameObject.getVertexList())});
    m_vertexBuffer = m_tgai.createBuffer({tga::BufferUsage::vertex, vertexListSize, m_vertexBufferStage});

    size_t indexListSize = m_gameObject.getIndexList().size() * sizeof(uint32_t);
    m_indexBufferStage = m_tgai.createStagingBuffer({indexListSize, tga::memoryAccess(m_gameObject.getIndexList())});
    m_indexBuffer = m_tgai.createBuffer({tga::BufferUsage::index, indexListSize, m_indexBufferStage});
}

void HybridRenderPipeline::_initPasses() {
    // 0 - geometry pass
    {
        // shaders
        tga::Shader vs = tga::loadShader(HYBRID_SHADER_PATH("game_object_vert.spv"), tga::ShaderType::vertex, m_tgai);
        tga::Shader fs = tga::loadShader(HYBRID_SHADER_PATH("game_object_frag.spv"), tga::ShaderType::fragment, m_tgai);

        // input layout (TODO: reuse)
        tga::InputLayout inputLayout({{{
            // S0
            tga::BindingType::uniformBuffer,  // B0: uniform buffer
            tga::BindingType::sampler         // B1: diffTex buffer
        }}});

        // pass
        m_geometryPass =
            m_tgai.createRenderPass(tga::RenderPassInfo{vs, fs}
                                        .setClearOperations(tga::ClearOperation::all)
                                        .setPerPixelOperations(tga::CompareOperation::less)
                                        .setRasterizerConfig({tga::FrontFace::counterclockwise, tga::CullMode::back})
                                        .setVertexLayout(hybrid::Vertex::layout())
                                        .setInputLayout(inputLayout)
                                        .setRenderTarget(m_gBuffer));

        // input sets
        m_geometryPassInputSets = {
            m_tgai.createInputSet({m_geometryPass,
                                   {
                                       {m_uniformBuffer, 0},
                                       {m_gameObject.getDiffuseTexture(), 1}
                                   },
                                   0}),
        };

        // free
        m_tgai.free(vs);
        m_tgai.free(fs);
    }

    // 1 - custom geometry pass (TODO: set at client side)
    {
        // shaders (TODO: reuse)
        tga::Shader vs =
            tga::loadShader(HYBRID_SHADER_PATH("full_screen_triangle_vert.spv"), tga::ShaderType::vertex, m_tgai);
        tga::Shader fs =
            tga::loadShader(HYBRID_SHADER_PATH("ray_march_test_frag.spv"), tga::ShaderType::fragment, m_tgai);

        // input layout (TODO: reuse)
        tga::InputLayout inputLayout({{{
            // S0
            tga::BindingType::uniformBuffer  // B0: uniform buffer
        }}});

        // pass
        m_customGeometryPass =
            m_tgai.createRenderPass(tga::RenderPassInfo{vs, fs}
                                        .setPerPixelOperations(tga::CompareOperation::less)
                                        .setRasterizerConfig({tga::FrontFace::clockwise, tga::CullMode::back})
                                        .setInputLayout(inputLayout)
                                        .setRenderTarget(m_gBuffer));

        // input sets
        m_customGeometryPassInputSets = {
            m_tgai.createInputSet({m_customGeometryPass,
                                   {
                                       {m_uniformBuffer, 0},
                                   },
                                   0}),
        };

        // free
        m_tgai.free(vs);
        m_tgai.free(fs);
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
        m_lightingPass =
            m_tgai.createRenderPass(tga::RenderPassInfo{vs, fs, m_window}
                                        .setRasterizerConfig({tga::FrontFace::clockwise, tga::CullMode::back})
                                        .setInputLayout(inputLayout)
                                        .setRenderTarget(m_window));

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

        // free
        m_tgai.free(vs);
        m_tgai.free(fs);
    }
}

void HybridRenderPipeline::_updateUniformData(const Camera& camera) {
    // update uniform data
    m_uniformData->projection = camera.getProjection();
    m_uniformData->view = camera.getView();
    m_uniformData->zBufferParams[0] = 1 - camera.getFarPlane() / camera.getNearPlane();
    m_uniformData->zBufferParams[1] = camera.getFarPlane() / camera.getNearPlane();
    m_uniformData->zBufferParams[2] = m_uniformData->zBufferParams[0] / camera.getFarPlane();
    m_uniformData->zBufferParams[3] = m_uniformData->zBufferParams[1] / camera.getFarPlane();
    m_uniformData->projectionParams[0] = camera.getNearPlane();
    m_uniformData->projectionParams[1] = camera.getFarPlane();
    m_uniformData->projectionParams[2] = 0;  // unused
    m_uniformData->projectionParams[3] = 0;  // unused
    m_uniformData->time = Time::getTime();
    m_uniformData->model = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0.5f)) * glm::scale(glm::mat4(1), glm::vec3(0.005f));  // to change
}

}  // namespace hybrid