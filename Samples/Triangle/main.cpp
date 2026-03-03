#include "HBE.h"
#include "TriangleScene.h"

using namespace HBE;
bool fullscreen = false;

void onAppUpdate(float delta)
{
    Input& input = *Application::instance->getInput();
    Window& window = *Application::instance->getWindow();
    if (input.getKeyDown(KEY_ESCAPE))
    {
        Application::instance->quit();
    }
    if (input.getKeyDown(KEY_F11))
    {
        fullscreen = !fullscreen;
        window.setFullscreen(fullscreen);
    }
    if (input.getKeyDown(KEY_V))
    {
        Configs::setVerticalSync(!Configs::getVerticalSync());
    }
}


int main()
{
    ApplicationInfo app_info{};
    app_info.app_name = "Triangle";
    Application app;
    app.init(app_info);
    //-----------------------SETUP--------------------
    {
        //-----------------------Games--------------------
        TriangleScene triangleScene = TriangleScene();
        //-----------------------EVENTS------------------
        event_subscription_id update_subscription_id;
        app.onUpdate.subscribe(update_subscription_id, &onAppUpdate);
        //-----------------------LOOP--------------------
        app.run();
        //-----------------------CLEANUP------------------
        app.onUpdate.unsubscribe(update_subscription_id);
        //-----------------------TERMINATE------------------
        //triangle scene deleted here.
    }
	app.release();
}
