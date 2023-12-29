#pragma once

#include "hybrid_shared.hpp"

namespace hybrid {

class Application {
public:
    Application();
    ~Application() = default;

    void run();

public:
    static Application& get() { return *s_instance; }

private:
    tga::Interface m_tgai;
    std::unique_ptr<tga::Window> m_window;

private:
    static Application *s_instance;
};

}  // namespace hybrid