#pragma once

#include "hybrid_shared.hpp"

namespace hybrid {

class Application {
public:
    Application();
    ~Application() = default;

    void run();
    tga::Interface& getInterface() { return m_tgai; }
    std::pair<uint32_t, uint32_t> getScreenResolution() const { return m_screenResolution; }
    float getDeltaTime() const { return m_deltaTime; }

public:
    static Application& get() { return *s_instance; }

private:
    tga::Interface m_tgai;
    std::unique_ptr<tga::Window> m_window;
    std::pair<uint32_t, uint32_t> m_screenResolution;
    float m_deltaTime = 0;

private:
    static Application *s_instance;
};

}  // namespace hybrid