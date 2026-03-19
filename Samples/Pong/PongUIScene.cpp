#include "PongUIScene.h"
#include "PongGame.h"

namespace Pong {
    void PongUIScene::OnWindowSizeChange(Window *window) {
        render_target.setResolution(window->getSize());
        score_left_entity.get<Transform>()->setPosition(vec3((window->getWidth() / 4.0), window->getHeight() - 20, 0));
        score_right_entity.get<Transform>()->setPosition(vec3(((window->getWidth() * 3) / 4.0), window->getHeight() - 20, 0));

        score_right_entity.get<Transform>()->setLocalScale(vec3(SCORE_TEXT_SIZE, SCORE_TEXT_SIZE, 1));
        score_right_entity.get<Transform>()->setLocalScale(vec3(SCORE_TEXT_SIZE, SCORE_TEXT_SIZE, 1));
    }

    Entity PongUIScene::createScore(PipelineInstance &pipeline_instance, Mesh *text) {
        Entity score = createEntity3D();
        MeshRenderer *score_renderer = score.attach<MeshRenderer>();
        score_renderer->pipeline_instance = pipeline_instance.getHandle();
        score_renderer->mesh = text->getHandle();
        score_renderer->ordered = true;
        score.get<Transform>()->setLocalScale(vec3(SCORE_TEXT_SIZE, SCORE_TEXT_SIZE, 1));

        return score;
    }

    PongUIScene::PongUIScene(PongGameState &game_state) : Scene() {
        this->game_state = &game_state;

        createResources();

        setupScene();

        this->onUpdate.subscribe(update_subscription_id, this, &PongUIScene::updateUI);

        window.onSizeChange.subscribe(on_window_size_change_subscription_id, this, &PongUIScene::OnWindowSizeChange);
    }

    PongUIScene::~PongUIScene() {
        this->onUpdate.unsubscribe(update_subscription_id);
        window.onSizeChange.unsubscribe(on_window_size_change_subscription_id);

        delete fps_counter;
    }


    void PongUIScene::createResources() {
        RasterizationTargetInfo render_target_info{};
        render_target_info.clear_color = vec4(0, 0, 0, 0);
        render_target_info.width = window.getWidth();
        render_target_info.height = window.getHeight();
        render_target_info.flags = RENDER_TARGET_FLAG_CLEAR_COLOR | RENDER_TARGET_FLAG_COLOR_ATTACHMENT;
        render_target.alloc(render_target_info);


        fps_counter = new FPSCounter(*this, &render_target);
        pipeline = &fps_counter->getPipeline();
        font = &fps_counter->getFont();


        vec2 size;
        Geometry::updateText(*Application::instance->getContext(),
                             score_left_mesh.getHandleRef(),
                             std::to_string(game_state->score_left),
                             *font,
                             1, 1, TEXT_ALIGNMENT_LEFT, PIVOT_TOP_LEFT, size);
        Geometry::updateText(*Application::instance->getContext(), score_right_mesh.getHandleRef(), std::to_string(game_state->score_right), *font, 1, 1, TEXT_ALIGNMENT_RIGHT, PIVOT_TOP_RIGHT, size);


        pipeline->allocInstance(left_text_pipeline_instance);
        pipeline->allocInstance(right_text_pipeline_instance);


        left_text_pipeline_instance.setUniform("material", &PongGame::LEFT_COLOR);
        right_text_pipeline_instance.setUniform("material", &PongGame::RIGHT_COLOR);
        left_text_pipeline_instance.setImage("mtsdf", font->getTextureAtlas()->getHandle());
        right_text_pipeline_instance.setImage("mtsdf", font->getTextureAtlas()->getHandle());
    }

    void PongUIScene::setupScene() {
        Entity camera_entity = createEntity3D();
        PixelCamera *camera = camera_entity.attach<PixelCamera>();
        camera->render_target = render_target.getHandle();

        score_left_entity = createScore(left_text_pipeline_instance,&score_left_mesh);
        score_right_entity = createScore(right_text_pipeline_instance, &score_right_mesh);

        last_score_left = game_state->score_left;
        last_score_right = game_state->score_right;

        score_left_entity.get<Transform>()->setPosition(vec3(((render_target.getResolution().x) / 4.0), render_target.getResolution().y - 20, 0));

        score_right_entity.get<Transform>()->setPosition(vec3(((render_target.getResolution().x * 3) / 4.0), render_target.getResolution().y - 20, 0));
    }

    void PongUIScene::updateUI(float delta) {
        if (last_score_left != game_state->score_left) {
            last_score_left = game_state->score_left;
            vec2 size;
            Geometry::updateText(context,score_left_mesh.getHandleRef(), std::to_string(game_state->score_left), *font, 1, 1, TEXT_ALIGNMENT_LEFT, PIVOT_TOP_CENTER, size);
        }
        if (last_score_right != game_state->score_right) {
            last_score_right = game_state->score_right;
            vec2 size;
            Geometry::updateText(context,score_left_mesh.getHandleRef(), std::to_string(game_state->score_right), *font, 1, 1, TEXT_ALIGNMENT_RIGHT, PIVOT_TOP_CENTER, size);
        }
    }
}
