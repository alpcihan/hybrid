#include "hybrid/renderer/hybrid_render_pipeline.hpp"

#include "hybrid/core/application.hpp"
#include "hybrid/core/time.hpp"

#define IMG_PYRAMID_SIZE 4

namespace hybrid {

HybridRenderPipeline::HybridRenderPipeline(tga::Window& window)
    : m_window(window), m_cmd(), m_tgai(Application::get().getInterface()) {
    _init();
}

void HybridRenderPipeline::render(const Camera& camera) {
    const auto [resX, resY] = Application::get().getScreenResolution();

    _updateUniformData(camera);

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

    auto nextFrame = m_tgai.nextFrame(m_window);
    auto cmdRecorder = tga::CommandRecorder{m_tgai, m_cmd};

    // buffer upload
    cmdRecorder
        .bufferUpload(m_uniformDataStage, m_uniformBuffer, sizeof(UniformData));
    
    // shadow map
    cmdRecorder
        .setComputePass(m_shadowPass)
        .bindInputSet(m_shadowInputSets[0])
        .bindInputSet(m_shadowInputSets[1])
        .dispatch((resX+31)/32,(resY+31)/32,1);

    // specular reflection map
    cmdRecorder
        .setComputePass(m_specularReflectionPass)
        .bindInputSet(m_specularReflectionPassInputSets[0])
        .bindInputSet(m_specularReflectionPassInputSets[1])
        .bindInputSet(m_specularReflectionPassInputSets[2])
        .dispatch((resX+31)/32, (resY+31)/32, 1);

    // specular reflection image pyramid
    for(int i = 0; i < IMG_PYRAMID_SIZE-1; i++) {
    cmdRecorder
        .barrier(tga::PipelineStage::ComputeShader, tga::PipelineStage::ComputeShader)
        .setComputePass(m_specularReflectionImgPyramidPass)
        .bindInputSet(m_specularReflectionImgPyramidPassInputSets[0])
        .bindInputSet(m_specularReflectionImgPyramidPassInputSets[1])
        .bindInputSet(m_specularReflectionImgPyramidPassInputSets[2])
        .bindInputSet(m_specularReflectionImgPyramidPassMipMapInputSets[i])
        .dispatch(1, 1, 1);
    }

    // lighting
    cmdRecorder
        .barrier(tga::PipelineStage::ComputeShader, tga::PipelineStage::FragmentShader)
        .setRenderPass(m_lightingPass, nextFrame)
        .bindInputSet(m_lightingPassInputSets[0])
        .bindInputSet(m_lightingPassInputSets[1])
        .bindInputSet(m_lightingPassInputSets[2])
        .draw(3, 0);

    // geometry pass
    cmdRecorder
        .setRenderPass(m_geometryPass, 0, {0, 0, 0, 0})
        .bindInputSet(m_geometryPassInputSets[0])
        .draw(3, 0);

    // custom geometry pass
    cmdRecorder
        .setRenderPass(m_customGeometryPass, 0)
        .bindInputSet(m_customGeometryPassInputSets[0])
        .draw(3, 0);

    // execute commands and show the result
    m_cmd = cmdRecorder.endRecording();
    m_tgai.execute(m_cmd);
    m_tgai.present(m_window, nextFrame);
}

void HybridRenderPipeline::_init() {
    _initResources();
    _initPasses();
}

void HybridRenderPipeline::_initResources() {
    const auto [resX, resY] = Application::get().getScreenResolution();

    // gbuffer
    m_gBuffer = {m_tgai.createTexture({resX, resY, tga::Format::r16g16b16a16_sfloat}),
                 m_tgai.createTexture({resX, resY, tga::Format::r16g16b16a16_sfloat}),
                 m_tgai.createTexture({resX, resY, tga::Format::r16g16b16a16_sfloat})};
    
    // shadow map (contains shadow coefficient)
    auto shadowStaging = m_tgai.createStagingBuffer({sizeof(float)*resX*resY});
    m_shadowMap = m_tgai.createBuffer({tga::BufferUsage::storage, {sizeof(float)*resX*resY},shadowStaging});
    // TODO: free staging buffer
  
    // init uniform buffer
    m_uniformDataStage = m_tgai.createStagingBuffer({sizeof(UniformData)});
    m_uniformData = static_cast<UniformData *>(m_tgai.getMapping(m_uniformDataStage));
    m_uniformBuffer = m_tgai.createBuffer({tga::BufferUsage::uniform, sizeof(UniformData)});

    // init specular reflection image pyramid (TODO: add min size check)
    m_specularReflectionImgPyramid.reserve(IMG_PYRAMID_SIZE);
    for(int i=0; i<IMG_PYRAMID_SIZE; ++i) {
        uint32_t scale = std::pow(2,i);
        m_specularReflectionImgPyramid.emplace_back(m_tgai.createTexture(
            {resX / scale, resY / scale, tga::Format::r16g16b16a16_sfloat}
        ));
        // std::cout << resX / scale << " " << resY / scale << "\n";
    }
}

void HybridRenderPipeline::_initPasses() {
    // geometry pass
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

    // custom geometry pass (TODO: set at client side)
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
  
    // shadow pass
    {
        // shader
        tga::Shader cs = tga::loadShader(HYBRID_SHADER_PATH("shadow_comp.spv"),tga::ShaderType::compute, m_tgai);

        // input layout
        tga::InputLayout inputLayout(
            {
                    //S0
                {
                    {tga::BindingType::uniformBuffer}
                },
                {   
                    //S1
                    {tga::BindingType::sampler},        // B0: gbuffer0
                    {tga::BindingType::sampler},        // B1: gbuffer1
                    {tga::BindingType::sampler},        // B2: gbuffer2
                    {tga::BindingType::storageBuffer}   // B3: shadowMap
                },
            }  
        );

        // pass
        m_shadowPass = m_tgai.createComputePass(tga::ComputePassInfo{cs,inputLayout});

        // input set
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
      
        // free
        m_tgai.free(cs);
    }

    // specular reflections pass
    {
        // shader
        tga::Shader cs = tga::loadShader(HYBRID_SHADER_PATH("specular_reflections_comp.spv"), tga::ShaderType::compute, m_tgai);

        // input layout (TODO: reuse)
        tga::InputLayout inputLayout({
            {
                // S0
                tga::BindingType::uniformBuffer  // B0: uniform buffer
            },
            {
                // S1
                {tga::BindingType::sampler},        // B0: gbuffer0
                {tga::BindingType::sampler},        // B1: gbuffer1
                {tga::BindingType::sampler},        // B2: gbuffer2
                {tga::BindingType::storageBuffer}   // B3: shadowMap
            },
            {
                // S2
                tga::BindingType::storageImage      // B0: specular reflection map
            }
        });

        // pass
        m_specularReflectionPass = m_tgai.createComputePass({cs, inputLayout});

        // input sets
        m_specularReflectionPassInputSets = {
            m_tgai.createInputSet({m_specularReflectionPass,
                                   {
                                       {m_uniformBuffer, 0},
                                   },
                                   0}),
            m_tgai.createInputSet({m_specularReflectionPass,
                                   {
                                       {m_gBuffer[0], 0},
                                       {m_gBuffer[1], 1},
                                       {m_gBuffer[2], 2},
                                       {m_shadowMap,  3}
                                   },
                                   1}),
            m_tgai.createInputSet({m_specularReflectionPass,
                                   {
                                       {m_specularReflectionImgPyramid[0], 0},
                                   },
                                   2}),

        };

        // free
        m_tgai.free(cs);
    }

    // specular reflections image pyramid pass
    {
        // shader
        tga::Shader cs = tga::loadShader(HYBRID_SHADER_PATH("mipmap_cross_bilateral_filter_comp.spv"), tga::ShaderType::compute, m_tgai);

        // input layout (TODO: reuse)
        tga::InputLayout inputLayout({
            {
                // S0
                {tga::BindingType::uniformBuffer}                   // B0: uniform buffer
            },
            {
                // S1
                {tga::BindingType::sampler},                        // B0: gbuffer0
                {tga::BindingType::sampler},                        // B1: gbuffer1
                {tga::BindingType::sampler}                         // B2: gbuffer2
            },
            {
                // S2
                {tga::BindingType::storageImage},                   // B0: specular reflection map
            },
            {   
                // S3
                {tga::BindingType::storageImage},                   // B0: specular pyramid sample
                {tga::BindingType::storageImage},                   // B1: specular pyramid down sample
            }
        });

        // pass
        m_specularReflectionImgPyramidPass = m_tgai.createComputePass({cs, inputLayout});

        // input sets
        m_specularReflectionImgPyramidPassInputSets.reserve(4); 

        // S0
        m_specularReflectionImgPyramidPassInputSets.emplace_back(
            m_tgai.createInputSet({m_specularReflectionImgPyramidPass,
                                   {
                                       {m_uniformBuffer, 0},
                                   },
                                   0}));
        // S1
        m_specularReflectionImgPyramidPassInputSets.emplace_back(                          
            m_tgai.createInputSet({m_specularReflectionImgPyramidPass,
                                   {
                                       {m_gBuffer[0], 0},
                                       {m_gBuffer[1], 1},
                                       {m_gBuffer[2], 2},
                                   },
                                   1}));
        // S2
        m_specularReflectionImgPyramidPassInputSets.emplace_back(                          
            m_tgai.createInputSet({m_specularReflectionImgPyramidPass,
                                   {
                                       {m_specularReflectionImgPyramid[0], 0},
                                   },
                                   2}));

        // S3s
        m_specularReflectionImgPyramidPassMipMapInputSets.reserve(IMG_PYRAMID_SIZE);

        for(int i = 0; i < IMG_PYRAMID_SIZE-1; i++) {
            m_specularReflectionImgPyramidPassMipMapInputSets.emplace_back(
                 m_tgai.createInputSet({m_specularReflectionImgPyramidPass,
                                   {
                                       {m_specularReflectionImgPyramid[i], 0},
                                       {m_specularReflectionImgPyramid[i+1], 1},
                                   },
                                   3})
            );
        }

        // free
        m_tgai.free(cs);
    }

    // lighting pass
    {
        // shader
        tga::Shader vs = tga::loadShader(HYBRID_SHADER_PATH("full_screen_triangle_vert.spv"), tga::ShaderType::vertex, m_tgai);
        tga::Shader fs = tga::loadShader(HYBRID_SHADER_PATH("pbr_frag.spv"), tga::ShaderType::fragment, m_tgai);

        // input layout (TODO: reuse)
        tga::InputLayout inputLayout({
            {
                // S0
                tga::BindingType::uniformBuffer     // B0: uniform buffer
            },
            {
                // S1
                {tga::BindingType::sampler},        // B0: gbuffer0
                {tga::BindingType::sampler},        // B1: gbuffer1
                {tga::BindingType::sampler},        // B2: gbuffer2
                {tga::BindingType::storageBuffer}   // B3: shadowMap 
            },
            {
                // S2
                {tga::BindingType::sampler,},       // B0: specular reflection map
                {tga::BindingType::sampler}         // B1: specular reflection pyramid
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
            m_tgai.createInputSet({m_lightingPass,
                                   {
                                       {m_specularReflectionImgPyramid[0], 0},
                                       {m_specularReflectionImgPyramid[IMG_PYRAMID_SIZE-1], 1}
                                   },
                                   2}),

        };

        // free
        m_tgai.free(vs);
        m_tgai.free(fs);
    }
}

void HybridRenderPipeline::_updateUniformData(const Camera& camera) {
    const auto [resX, resY] = Application::get().getScreenResolution();
  
    // update uniform data
    m_uniformData->projection = camera.getProjection();
    m_uniformData->view = camera.getView();
    m_uniformData->zBufferParams[0] = 1 - camera.getFarPlane() / camera.getNearPlane();
    m_uniformData->zBufferParams[1] = camera.getFarPlane() / camera.getNearPlane();
    m_uniformData->zBufferParams[2] = m_uniformData->zBufferParams[0] / camera.getFarPlane();
    m_uniformData->zBufferParams[3] = m_uniformData->zBufferParams[1] / camera.getFarPlane();
    m_uniformData->projectionParams[0] = camera.getNearPlane();
    m_uniformData->projectionParams[1] = camera.getFarPlane();
    m_uniformData->projectionParams[2] = resX;
    m_uniformData->projectionParams[3] = resY;
    m_uniformData->time = Time::getTime();
}

}  // namespace hybrid