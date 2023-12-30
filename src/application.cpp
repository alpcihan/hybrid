#pragma once

#include "hybrid/application.hpp"
#include "hybrid/hybrid_render_pipeline.hpp"
#include "hybrid/timer.hpp"
#include "hybrid/perspective_camera.hpp"

namespace hybrid {

Application::Application() : m_tgai() {
    // instance
    s_instance = this;

    // window
    m_screenResolution = std::pair<uint32_t, uint32_t>(640, 360);
    m_window = std::make_unique<tga::Window>(m_tgai.createWindow({m_screenResolution.first, m_screenResolution.second}));

    // camera
    m_camera = std::make_unique<PerspectiveCamera>(30, float(m_screenResolution.first)/m_screenResolution.second);
    m_cameraController = std::make_unique<CameraController>(*m_camera);
}

void Application::run() {
    
    m_tgai.setWindowTitle(*m_window, "hybrid");

    std::unique_ptr<HybridRenderPipeline> renderer = std::make_unique<HybridRenderPipeline>(*m_window); 

    Timer timer;
    while (!m_tgai.windowShouldClose(*m_window)) {
        std::cout << 1 / m_deltaTime << "\n";
        m_deltaTime = timer.elapsed();
        timer.reset();
        
        m_cameraController->update(m_deltaTime);
        renderer->render(*m_camera);
    }
}

Application *Application::s_instance = nullptr;

}  // namespace hybrid