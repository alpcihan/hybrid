#pragma once

#include "hybrid/application.hpp"

namespace hybrid {

Application::Application() : m_tgai() {
    // instance
    s_instance = this;

    // window
    m_window = std::make_unique<tga::Window>(m_tgai.createWindow({640, 320}));
}

Application::~Application() {}

void Application::run() {
    while (!m_tgai.windowShouldClose(*m_window)) {}
}

Application *Application::s_instance = nullptr;

}  // namespace hybrid