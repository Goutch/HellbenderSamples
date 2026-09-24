#include "RaytracingScene.h"

#include "HBE.h"

using namespace HBE;
bool fullscreen = false;
Scene *main_scene;
Scene *ui_scene;

void onAppUpdate(float delta) {
	Input &input = *Application::instance->getInput();
	if (input.getKeyDown(KEY_ESCAPE)) {
		Application::instance->quit();
	}
	if (input.getKeyDown(KEY_F11)) {
		fullscreen = !fullscreen;
		Application::instance->getWindow()->setFullscreen(fullscreen);
	}
	if (input.getKeyDown(KEY_V)) {
		Configs::setVerticalSync(!Configs::getVerticalSync());
	}

	if (input.getKeyDown(KEY_B)) {
		if (ui_scene != nullptr) {
			ui_scene->setActive(!ui_scene->isActive());
		}
	}
	if (input.getKeyDown(KEY_C)) {
		Entity cam = main_scene->getCameraEntity();
		if (cam.valid()) {
			if (cam.has<CameraController>()) {
				input.setCursorVisible(true);
				cam.detach<CameraController>();
			} else {
				input.setCursorVisible(false);
				cam.attach<CameraController>();
			}
		}
	}
}

void onAppPresent() {
	std::vector<ImageHandle> present_images;
	if (ui_scene != nullptr && ui_scene->isActive() && ui_scene->getMainCameraTexture() != HBE_NULL_HANDLE) {
		present_images.push_back(ui_scene->getMainCameraTexture());
	}
	if (main_scene != nullptr && main_scene->isActive() && main_scene->getMainCameraTexture() != HBE_NULL_HANDLE) {
		present_images.push_back(main_scene->getMainCameraTexture());
	}

	PresentCmdInfo present_info;
	present_info.image_count = present_images.size();
	present_info.images = present_images.data();
	Application::instance->getContext()->cmdPresent(present_info);
}

int main() {
	ApplicationInfo app_info{};
	app_info.context_info.vulkan_version = VULKAN_VERSION_1_1;
	app_info.context_info.required_extension_flags = VULKAN_REQUIRED_EXTENSION_RTX |
	                                                 VULKAN_REQUIRED_EXTENSION_DESCRIPTOR_INDEXING;
	app_info.window_info.startSize = vec2i(1920, 1080);
	app_info.app_name = "Raytracer";
	Application app;
	app.init(app_info);
	//-----------------------SETUP--------------------
	main_scene = new RaytracingScene();

	//-----------------------EVENTS------------------
	event_subscription_id update_subscription_id;
	event_subscription_id present_subscription_id;
	Application::instance->onUpdate.subscribe(update_subscription_id, &onAppUpdate);
	Application::instance->onPresent.subscribe(present_subscription_id, &onAppPresent);
	//-----------------------LOOP--------------------
	Application::instance->run();
	//-----------------------CLEANUP------------------
	Application::instance->onUpdate.unsubscribe(update_subscription_id);
	Application::instance->onPresent.unsubscribe(present_subscription_id);
	//-----------------------TERMINATE------------------

	if (ui_scene != nullptr)
		delete ui_scene;
	if (main_scene != nullptr)
		delete main_scene;
	Application::instance->terminate();
}