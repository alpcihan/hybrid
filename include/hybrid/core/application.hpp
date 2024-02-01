#pragma once

#include "hybrid/hybrid_shared.hpp"
#include "hybrid/renderer/camera.hpp"
#include "hybrid/core/game_object.hpp"
#include "hybrid/renderer/hybrid_render_pipeline.hpp"

namespace hybrid {

class Application {
public:
    ~Application() = default;
    Application(Application &other) = delete;
    void operator=(const Application &) = delete;

    void init(const std::string& name, uint32_t width, uint32_t height);
    void run(void (*onUpdate)() = nullptr);

    tga::Interface& getInterface() { return m_tgai; }
    tga::Window getWindow() { return *m_window; }
    std::pair<uint32_t, uint32_t> getScreenResolution() const { return m_screenResolution; }

    void setGameObject(std::shared_ptr<GameObject> gameObject) { m_gameObject = gameObject; }
    void setCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }
    void setHDRI(tga::Texture hdri) { m_hdri = hdri; }

public:
    static Application& get();

private:
    tga::Interface m_tgai;

    // window
    std::unique_ptr<tga::Window> m_window;
    std::pair<uint32_t, uint32_t> m_screenResolution;
    
    // camera
    std::shared_ptr<Camera> m_camera;

    // scene (TODO: create a scene system)
    std::shared_ptr<GameObject> m_gameObject;
    tga::Texture m_hdri;

protected:
    Application();
    
private:
    static Application *s_instance;
};

}  // namespace hybrid