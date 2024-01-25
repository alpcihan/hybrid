#pragma once

#include "hybrid/hybrid_shared.hpp"
#include "hybrid/renderer/camera.hpp"
#include "hybrid/renderer/game_object.hpp"

namespace hybrid {

class HybridRenderPipeline {
public:
    HybridRenderPipeline(tga::Window& window, GameObject& gameObject);

    void render(const Camera& camera);

private:
    struct UniformData {
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec4 zBufferParams;
        glm::vec4 projectionParams;
        float time;
    };

    struct ModelData {
        glm::mat4 model;
    };

private:
    tga::Interface& m_tgai;

    tga::Window m_window;
    tga::CommandBuffer m_cmd;

    // scene maps
    std::vector<tga::Texture> m_gBuffer;
    tga::Buffer m_shadowMap;
    std::vector<tga::Texture> m_specularReflectionImgPyramid;
    tga::Texture m_skybox;
    tga::Texture m_sceneTargetMap;
  
    tga::Texture m_testTexture;


    // uniform buffer
    UniformData *m_uniformData;
    tga::StagingBuffer m_uniformDataStage;
    tga::Buffer m_uniformBuffer;

    // scene data
    GameObject& m_gameObject;

    // model data
    ModelData* m_modelData;
    tga::StagingBuffer m_modelDataStage;
    tga::Buffer m_modelBuffer;

    // scene buffers
    tga::StagingBuffer m_vertexBufferStage;
    tga::Buffer m_vertexBuffer;
    tga::StagingBuffer m_indexBufferStage;
    tga::Buffer m_indexBuffer;

    //bvh
    tga::ext::BottomLevelAccelerationStructure m_blas;
    tga::ext::TopLevelAccelerationStructure m_tlas;

    // passes
    tga::RenderPass m_geometryPass;
    tga::RenderPass m_customGeometryPass;
    tga::ComputePass m_shadowPass;
    tga::ComputePass m_specularReflectionPass;
    tga::ComputePass m_specularReflectionImgPyramidPass;
    tga::RenderPass m_lightingPass;
    tga::RenderPass m_skyboxPass;
    tga::RenderPass m_scenePresentPass;

    // input sets
    std::vector<tga::InputSet> m_geometryPassInputSets;
    std::vector<tga::InputSet> m_customGeometryPassInputSets;
    std::vector<tga::InputSet> m_shadowInputSets;
    std::vector<tga::InputSet> m_specularReflectionPassInputSets;
    std::vector<tga::InputSet> m_specularReflectionImgPyramidPassInputSets;
    std::vector<tga::InputSet> m_specularReflectionImgPyramidPassMipMapInputSets;
    std::vector<tga::InputSet> m_lightingPassInputSets;
    std::vector<tga::InputSet> m_skyboxPassInputSets;
    std::vector<tga::InputSet> m_scenePresentPassInputSets;

private:
    void _init();
    void _initResources();
    void _initPasses();
    void _updateUniformData(const Camera& camera);
    void _updateModelData();
};

}  // namespace hybrid