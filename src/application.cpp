#pragma once

#include "hybrid/application.hpp"
#include "hybrid/renderer.hpp"
#include "hybrid/timer.hpp"

namespace hybrid {

Application::Application() : m_tgai() {
    // instance
    s_instance = this;

    // window
    m_screenResolution = std::pair<uint32_t, uint32_t>(640, 360);
    m_window = std::make_unique<tga::Window>(m_tgai.createWindow({m_screenResolution.first, m_screenResolution.second}));
}

void Application::run() {
    
    m_tgai.setWindowTitle(*m_window, "hybrid");

    std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>(*m_window); 

    Timer timer;
    while (!m_tgai.windowShouldClose(*m_window)) {
        m_deltaTime = timer.elapsed();
        timer.reset();
        
        renderer->renderFrame();
    }
}

Application *Application::s_instance = nullptr;

}  // namespace hybrid