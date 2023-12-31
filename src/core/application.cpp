#include "hybrid/core/application.hpp"

#include "hybrid/core/time.hpp"
#include "hybrid/core/timer.hpp"
#include "hybrid/renderer/hybrid_render_pipeline.hpp"
#include "hybrid/renderer/perspective_camera.hpp"

namespace hybrid {

Application *Application::s_instance = nullptr;

Application::Application() : m_tgai() {
    // instance
    s_instance = this;

    // window
    m_screenResolution = std::pair<uint32_t, uint32_t>(640, 360);
    m_window =
        std::make_unique<tga::Window>(m_tgai.createWindow({m_screenResolution.first, m_screenResolution.second}));

    // camera
    m_camera = std::make_unique<PerspectiveCamera>(30, float(m_screenResolution.first) / m_screenResolution.second);
    m_cameraController = std::make_unique<CameraController>(*m_camera);
}

void Application::run() {
    m_tgai.setWindowTitle(*m_window, "hybrid");

    std::unique_ptr<HybridRenderPipeline> renderPipeline = std::make_unique<HybridRenderPipeline>(*m_window);

    Timer timer;
    while (!m_tgai.windowShouldClose(*m_window)) {
        const float deltaTime = timer.elapsed();
        // update time
        {
            std::cout << "FPS: " << 1 / deltaTime << "\n";
            Time::_update(deltaTime);
            timer.reset();
        }

        m_cameraController->update(deltaTime);
        renderPipeline->render(*m_camera);

        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

}  // namespace hybrid