#include "hybrid/hybrid.hpp"

#define APP_NAME "Spaceship Example"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 360

#define OBJ_PATH "FA59AMako/FA59AMako.obj"
#define OBJ_BASE_COLOR_MAP_PATH "FA59AMako/FA59AMako_BaseColor.png"
#define OBJ_GLOSSINESS_MAP_PATH "FA59AMako/FA59AMako_Glossiness.png"
#define OBJ_ROUGHNESS_MAP_PATH "FA59AMako/FA59AMako_Roughness.png"

void onUpdate() {
    std::cout << "Fps: " << 1 / hybrid::Time::getDeltaTime() << "\n">;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

int main() {
    // init the application
    hybrid::Application& app = hybrid::Application::get();
    app.init(APP_NAME, WINDOW_WIDTH, WINDOW_HEIGHT);

    // load the game object (currently only single game object is supported)
    std::shared_ptr<hybrid::GameObject> gameObject = std::make_shared<hybrid::GameObject>(
        HYBRID_ASSET_PATH(OBJ_PATH),
        HYBRID_ASSET_PATH(OBJ_BASE_COLOR_MAP_PATH),
        HYBRID_ASSET_PATH(OBJ_GLOSSINESS_MAP_PATH),
        HYBRID_ASSET_PATH(OBJ_ROUGHNESS_MAP_PATH));
    app.setGameObject(gameObject);

    app.run(onUpdate);

    return 0;
}