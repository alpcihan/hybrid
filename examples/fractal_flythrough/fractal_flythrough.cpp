#include "hybrid/hybrid.hpp"
#include "model_controller.hpp"

// application macros
#define APP_NAME "Fractal Flythrough"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define ASSET_PATH(relative) HYBRID_EXAMPLES_PATH("assets/" relative)

// camera macros
#define CAMERA_FOV 30
#define CAMERA_BACK_OFFSET 2.f
#define CAMERA_UP_OFFSET 0.2f

// model macros
#define OBJ_PATH "models/FA59AMako/FA59AMako.obj"
#define OBJ_BASE_COLOR_MAP_PATH "models/FA59AMako/FA59AMako_BaseColor.png"
#define OBJ_GLOSSINESS_MAP_PATH "models/FA59AMako/FA59AMako_Glossiness.png"
#define OBJ_ROUGHNESS_MAP_PATH "models/FA59AMako/FA59AMako_Roughness.png"

// hdri macros
#define HDRI_PATH "HDRIs/starry_night_sky_dome_4k.hdr"

std::shared_ptr<hybrid::Camera> camera;
std::shared_ptr<hybrid::GameObject> plane;
std::unique_ptr<ModelController> planeController;

// application loop callback
void onUpdate() {
    const float deltaTime = hybrid::Time::getDeltaTime();

    planeController->update(deltaTime);

    // track the plane with camera
    glm::vec3 targetCameraPosition = planeController->getPosition() -
                                     planeController->getLookDir() * CAMERA_BACK_OFFSET +
                                     planeController->getUpDir() * CAMERA_UP_OFFSET;
    glm::vec3 cameraPosition = glm::mix(glm::vec3(camera->getView()[3]), targetCameraPosition, 0.99f);
    camera->setView(glm::lookAt(cameraPosition, planeController->getPosition(), planeController->getWorldUp()));
}

int main() {
    // create application
    hybrid::Application& app = hybrid::Application::get();
    app.init(APP_NAME, WINDOW_WIDTH, WINDOW_HEIGHT);

    // load plane model (currently only single model loading is supported)
    plane = std::make_shared<hybrid::GameObject>(
        ASSET_PATH(OBJ_PATH),
        ASSET_PATH(OBJ_BASE_COLOR_MAP_PATH),
        ASSET_PATH(OBJ_GLOSSINESS_MAP_PATH),
        ASSET_PATH(OBJ_ROUGHNESS_MAP_PATH));
    app.setGameObject(plane);

    // create camera
    const float ASPECT_RATIO = (float)WINDOW_WIDTH / WINDOW_HEIGHT;
    camera = std::make_shared<hybrid::PerspectiveCamera>(CAMERA_FOV, ASPECT_RATIO);
    app.setCamera(camera);

    // create controllers
    planeController = std::make_unique<ModelController>(plane);

    // create hdri
    tga::Texture hdri = tga::loadTexture(ASSET_PATH(HDRI_PATH), tga::Format::r32g32b32a32_sfloat, tga::SamplerMode::linear, app.getInterface(), false);
    app.setHDRI(hdri);

    app.run(onUpdate);

    return 0;
}