#include "hybrid/core/application.hpp"

#include "hybrid/core/time.hpp"
#include "hybrid/core/timer.hpp"
#include "hybrid/renderer/hybrid_render_pipeline.hpp"
#include "hybrid/renderer/perspective_camera.hpp"

namespace hybrid {

Application *Application::s_instance = nullptr;

Application& Application::get() {
    if(s_instance == nullptr) {
        s_instance = new Application();
    }

    return *s_instance; 
}

void Application::init(const std::string& name, uint32_t width, uint32_t height) {
    // window
    m_screenResolution = std::pair<uint32_t, uint32_t>(width, height);
    m_window = std::make_unique<tga::Window>(m_tgai.createWindow({m_screenResolution.first, m_screenResolution.second}));
    m_tgai.setWindowTitle(*m_window, name);
}

void Application::run(void (*onUpdate)()) {
    std::unique_ptr<HybridRenderPipeline> renderPipeline =
        std::make_unique<HybridRenderPipeline>(*m_window, *m_gameObject);
    
    Timer timer;
    while (!m_tgai.windowShouldClose(*m_window)) {
        // update time
        const float deltaTime = timer.elapsed();
        timer.reset();  
        Time::_update(deltaTime);
        
        // user callback
        if(onUpdate) onUpdate();

        renderPipeline->render(*m_camera);
    }
}

Application::Application() : m_tgai() {}

}  // namespace hybrid