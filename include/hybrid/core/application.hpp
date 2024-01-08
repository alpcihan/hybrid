#pragma once

#include "hybrid/hybrid_shared.hpp"
#include "hybrid/renderer/camera.hpp"
#include "hybrid/renderer/camera_controller.hpp"
#include "hybrid/renderer/game_object.hpp"

namespace hybrid {

class Application {
public:
    Application();
    ~Application() = default;

    void run();

    tga::Interface& getInterface() { return m_tgai; }
    tga::Window getWindow() { return *m_window; }
    std::pair<uint32_t, uint32_t> getScreenResolution() const { return m_screenResolution; }

public:
    static Application& get() { return *s_instance; }

private:
    tga::Interface m_tgai;

    // window
    std::unique_ptr<tga::Window> m_window;
    std::pair<uint32_t, uint32_t> m_screenResolution;
    
    // camera
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<CameraController> m_cameraController;

    // scene
    std::unique_ptr<GameObject> m_gameObject;

private:
    static Application *s_instance;
};

}  // namespace hybrid