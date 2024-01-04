#include "hybrid/renderer/hybrid_render_pipeline.hpp"

#include "hybrid/core/application.hpp"
#include "hybrid/core/time.hpp"

namespace hybrid {

HybridRenderPipeline::HybridRenderPipeline(tga::Window& window)
    : m_window(window), m_cmd(), m_tgai(Application::get().getInterface()) {
    _init();
}

void HybridRenderPipeline::render(const Camera& camera) {
    _updateUniformData(camera);

    auto nextFrame = m_tgai.nextFrame(m_window);

    static bool isInit = true;

    if(isInit) {
        m_cmd = tga::CommandRecorder{m_tgai, m_cmd}  
            // rp1 + buffer upload
            .bufferUpload(m_uniformDataStage, m_uniformBuffer, sizeof(UniformData))
            .setRenderPass(m_geometryPass, 0, {0, 0, 0, 0})
            .bindInputSet(m_geometryPassInputSets[0])
            .draw(3, 0)
            // rp 2
            .setRenderPass(m_customGeometryPass, 0)
            .bindInputSet(m_customGeometryPassInputSets[0])
            .draw(3, 0)
            .endRecording();
        m_tgai.execute(m_cmd);
        isInit = false;
    }

    m_cmd = tga::CommandRecorder{m_tgai, m_cmd}  // 0 - buffer update
                .bufferUpload(m_uniformDataStage, m_uniformBuffer, sizeof(UniformData))

                // shadow pass
                .setComputePass(m_shadowPass)
                .bindInputSet(m_shadowInputSets[0])
                .bindInputSet(m_shadowInputSets[1])
                .dispatch((m_res.first+31)/32,(m_res.second+31)/32,1)
                .barrier(tga::PipelineStage::ComputeShader, tga::PipelineStage::FragmentShader)

                // lighting pass
                .setRenderPass(m_lightingPass, nextFrame)
                .bindInputSet(m_lightingPassInputSets[0])
                .bindInputSet(m_lightingPassInputSets[1])
                .draw(3, 0)

                // geometry pass
                .setRenderPass(m_geometryPass, 0, {0, 0, 0, 0})
                .bindInputSet(m_geometryPassInputSets[0])
                .draw(3, 0)
                // custom geometry pass
                .setRenderPass(m_customGeometryPass, 0)
                .bindInputSet(m_customGeometryPassInputSets[0])
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
    m_res = Application::get().getScreenResolution();

    // init gbuffer
    m_gBuffer = {m_tgai.createTexture({m_res.first, m_res.second, tga::Format::r16g16b16a16_sfloat}),
                 m_tgai.createTexture({m_res.first, m_res.second, tga::Format::r16g16b16a16_sfloat}),
                 m_tgai.createTexture({m_res.first, m_res.second, tga::Format::r16g16b16a16_sfloat})};

    // shadow map (contains shadow coefficient)
    auto shadowStaging = m_tgai.createStagingBuffer({sizeof(float)*m_res.first*m_res.second});
    m_shadowMap = m_tgai.createBuffer({tga::BufferUsage::storage, {sizeof(float)*m_res.first*m_res.second},shadowStaging});

    // init uniform buffer
    m_uniformDataStage = m_tgai.createStagingBuffer({sizeof(UniformData)});
    m_uniformData = static_cast<UniformData *>(m_tgai.getMapping(m_uniformDataStage));
    m_uniformBuffer = m_tgai.createBuffer({tga::BufferUsage::uniform, sizeof(UniformData), m_uniformDataStage});
}

void HybridRenderPipeline::_initPasses() {
    // 0 - geometry pass
    {
        // shaders
        tga::Shader vs = tga::loadShader(HYBRID_SHADER_PATH("geometry_vert.spv"), tga::ShaderType::vertex, m_tgai);
        tga::Shader fs = tga::loadShader(HYBRID_SHADER_PATH("geometry_frag.spv"), tga::ShaderType::fragment, m_tgai);

        // input layout (TODO: reuse)
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

        // free
        m_tgai.free(vs);
        m_tgai.free(fs);
    }

    // 1 - custom geometry pass (TODO: set at client side)
    {
        // shaders (TODO: reuse)
        tga::Shader vs = tga::loadShader(HYBRID_SHADER_PATH("full_screen_triangle_vert.spv"), tga::ShaderType::vertex, m_tgai);
        tga::Shader fs = tga::loadShader(HYBRID_SHADER_PATH("ray_march_test_frag.spv"), tga::ShaderType::fragment, m_tgai);

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

     //shadow pass
    {
        //shader
        tga::Shader cs = tga::loadShader(HYBRID_SHADER_PATH("shadow_comp.spv"),tga::ShaderType::compute, m_tgai);

        //input layout
        tga::InputLayout inputLayout(
            {
                    //S0
                {
                    {tga::BindingType::uniformBuffer}
                },
                {   
                    //S1
                    {tga::BindingType::sampler, 1},  // B0: gbuffer0
                    {tga::BindingType::sampler, 1},  // B1: gbuffer1
                    {tga::BindingType::sampler, 1},  // B2: gbuffer2
                    {tga::BindingType::storageBuffer}// B3: shadowMap
                },
            }  
        );

        //pass
        m_shadowPass = m_tgai.createComputePass(tga::ComputePassInfo{cs,inputLayout});

        //input set
        m_shadowInputSets = {
            m_tgai.createInputSet({m_shadowPass,
                                   {
                                       {m_uniformBuffer, 0},
                                   },
                                   0}),
            m_tgai.createInputSet({m_shadowPass,
                                   {
                                       {m_gBuffer[0], 0},
                                       {m_gBuffer[1], 1},
                                       {m_gBuffer[2], 2},
                                       {m_shadowMap,  3}
                                   },
                                   1})
            };
        
        m_tgai.free(cs);
    }

    // lighting pass
    {
        // shader
        tga::Shader vs = tga::loadShader(HYBRID_SHADER_PATH("full_screen_triangle_vert.spv"), tga::ShaderType::vertex, m_tgai);
        tga::Shader fs = tga::loadShader(HYBRID_SHADER_PATH("pbr_frag.spv"), tga::ShaderType::fragment, m_tgai);

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
                {tga::BindingType::sampler, 1},  // B2: gbuffer2
                {tga::BindingType::storageBuffer}// B3: shadowMap 
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
                                       {m_shadowMap,  3}
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
    m_uniformData->projectionParams[2] = m_res.first;  // unused
    m_uniformData->projectionParams[3] = m_res.second;  // unused
    m_uniformData->time = Time::getTime();
}

}  // namespace hybrid