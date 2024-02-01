#include "hybrid/hybrid.hpp"
#include "model_controller.hpp"

// application properties
#define APP_NAME "Fractal Flythrough"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 360

// camera properties
#define CAMERA_FOV 30
#define CAMERA_BACK_OFFSET 2.f
#define CAMERA_UP_OFFSET 0.2f

// model properties
#define OBJ_PATH "FA59AMako/FA59AMako.obj"
#define OBJ_BASE_COLOR_MAP_PATH "FA59AMako/FA59AMako_BaseColor.png"
#define OBJ_GLOSSINESS_MAP_PATH "FA59AMako/FA59AMako_Glossiness.png"
#define OBJ_ROUGHNESS_MAP_PATH "FA59AMako/FA59AMako_Roughness.png"

std::shared_ptr<hybrid::Camera> camera;
std::shared_ptr<hybrid::GameObject> plane;
std::unique_ptr<ModelController> planeController;

// application loop callback
void onUpdate() {
    const float deltaTime = hybrid::Time::getDeltaTime();

    // update the plane controller
    planeController->update(deltaTime);

    // track the plane with camera
    glm::vec3 targetCameraPosition = planeController->getPosition() -
                                     planeController->getLookDir() * CAMERA_BACK_OFFSET +
                                     planeController->getUpDir() * CAMERA_UP_OFFSET;

    // smoothly interpolate between the current and target camera positions
    glm::vec3 cameraPosition = glm::mix(glm::vec3(camera->getView()[3]), targetCameraPosition, 0.99f);
    camera->setView(glm::lookAt(cameraPosition, planeController->getPosition(), planeController->getWorldUp()));

    // display fps
    std::cout << "Fps: " << 1 / deltaTime << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

int main() {
    // create application
    hybrid::Application& app = hybrid::Application::get();
    app.init(APP_NAME, WINDOW_WIDTH, WINDOW_HEIGHT);

    // load plane model (currently only single model loading is supported)
    plane = std::make_shared<hybrid::GameObject>(
        HYBRID_ASSET_PATH(OBJ_PATH), HYBRID_ASSET_PATH(OBJ_BASE_COLOR_MAP_PATH),
        HYBRID_ASSET_PATH(OBJ_GLOSSINESS_MAP_PATH), HYBRID_ASSET_PATH(OBJ_ROUGHNESS_MAP_PATH));
    app.setGameObject(plane);

    // create camera
    const float ASPECT_RATIO = (float)WINDOW_WIDTH / WINDOW_HEIGHT;
    camera = std::make_shared<hybrid::PerspectiveCamera>(CAMERA_FOV, ASPECT_RATIO);
    app.setCamera(camera);

    // create controllers
    planeController = std::make_unique<ModelController>(plane);

    app.run(onUpdate);

    return 0;
}