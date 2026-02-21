
#include "HBE.h"
#include "MenuScene.h"

using namespace HBE;
bool fullscreen = false;

void onAppUpdate(float delta) {
	if (Input::getKeyDown(KEY_ESCAPE)) {
		Application::quit();
	}
	if (Input::getKeyDown(KEY_F11)) {
		fullscreen = !fullscreen;
		Graphics::getWindow()->setFullscreen(fullscreen);
	}
	if (Input::getKeyDown(KEY_V)) {
		Configs::setVerticalSync(!Configs::getVerticalSync());
	}
}


int main() {
	ApplicationInfo app_info{};
	app_info.name = "Menu";
	Application::init(app_info);
	//-----------------------SETUP--------------------
	{
		//-----------------------Games--------------------
		MenuScene menu_scene = MenuScene();
		//-----------------------EVENTS------------------
		event_subscription_id update_subscription_id;
		Application::onUpdate.subscribe(update_subscription_id,&onAppUpdate);
		//-----------------------LOOP--------------------
		Application::run();
		//-----------------------CLEANUP------------------
		Application::onUpdate.unsubscribe(update_subscription_id);
		//-----------------------TERMINATE------------------
	}
	Application::terminate();
}