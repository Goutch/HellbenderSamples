#include "PongGameScene.h"
#include "Systems/BallSystem.h"
#include "Systems/PaddleSystem.h"
#include "PongGame.h"

namespace Pong {
    Area &PongGameScene::getArea() {
        return game_area;
    }

    PongGameScene::PongGameScene(PongGameState &game_state) : Scene() {
        createResources();
        addSystem(new BallSystem(this, game_state, &bounce_sound_instance, &render_target));
        addSystem(new PaddleSystem(this));
        setupScene();
        window.onSizeChange.subscribe(on_window_size_change_subscription_id, this, &PongGameScene::OnWindowSizeChange);
    }

    PongGameScene::~PongGameScene() {
        window.onSizeChange.unsubscribe(on_window_size_change_subscription_id);
    }

    Entity PongGameScene::createBall(vec2 position, vec2 velocity) {
        Entity ball = createEntity3D();

        BallComponent *ballComponent = ball.attach<BallComponent>();
        ballComponent->velocity = velocity;
        ballComponent->radius = 0.2;
        ball.get<Transform>()->setPosition(vec3(position, 0));
        ball.get<Transform>()->setLocalScale(vec3(ballComponent->radius));

        return ball;
    }

    Entity PongGameScene::createPaddle(vec3 position, KEY up_key, KEY down_key, PipelineInstance *paddle_pipeline_instance) {
        Entity paddle = createEntity3D();
        MeshRenderer *paddle_renderer = paddle.attach<MeshRenderer>();
        paddle_renderer->mesh = quad_mesh.getHandle();
        paddle_renderer->layer = 0;
        paddle_renderer->pipeline_instance = paddle_pipeline_instance->getHandle();

        PaddleComponent *paddleComponent = paddle.attach<PaddleComponent>();
        paddleComponent->speed = 10;
        paddleComponent->size_x = 0.3;
        paddleComponent->size_y = 2.0;
        paddleComponent->down_key = down_key;
        paddleComponent->up_key = up_key;

        paddle.get<Transform>()->setPosition(position);

        return paddle;
    }

    void PongGameScene::setupScene() {
        Entity camera_entity = createEntity3D();
        Camera2D *camera = camera_entity.attach<Camera2D>();
        camera->render_target = render_target.getHandle();

        for (int i = 0; i < 1; ++i) {
            createBall(vec2(0, 0),
                       vec2(Random::floatRange(-10, 10), Random::floatRange(-10, 10)));
        }
        //createBall(vec2(0, 0), vec2(10, 10));
        paddle_left_entity = createPaddle(vec3{-game_area.size.x / 2 + 1, 0, 0},
                                          KEY_W,
                                          KEY_S,
                                          &paddle_left_pipeline_instance);
        paddle_right_entity = createPaddle(vec3{game_area.size.x / 2 - 1, 0, 0},
                                           KEY_UP,
                                           KEY_DOWN,
                                           &paddle_right_pipeline_instance);
        onRenderTargetResolutionChange(&render_target);
    }

    void PongGameScene::createResources() {
        RasterizationTargetInfo render_target_info{};
        render_target_info.width = window.getWidth();
        render_target_info.height = window.getHeight();
        render_target_info.flags = RENDER_TARGET_FLAG_COLOR_ATTACHMENT | RENDER_TARGET_FLAG_CLEAR_COLOR;
        render_target.alloc(render_target_info);

        MeshInfo mesh_info{};
        mesh_info.attribute_info_count = 1;
        mesh_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
        quad_mesh.alloc(mesh_info);
        Geometry::createQuad(quad_mesh, 1, 1, VERTEX_FLAG_NONE);

        vertex_shader.loadGLSL("shaders/defaults/Position.vert", SHADER_STAGE_VERTEX);
        fragment_shader.loadGLSL("shaders/defaults/Position.frag", SHADER_STAGE_FRAGMENT);

        RasterizationPipelineInfo pipeline_info{};
        pipeline_info.attribute_info_count = 1;
        pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
        pipeline_info.vertex_shader = vertex_shader.getHandle();
        pipeline_info.fragment_shader = fragment_shader.getHandle();
        pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_NONE;
        pipeline_info.rasterization_target = render_target.getHandle();
        pipeline.alloc(pipeline_info);
        pipeline.allocInstance(paddle_left_pipeline_instance);
        pipeline.allocInstance(paddle_right_pipeline_instance);

        vec4 color = {1, 1, 1, 1};
        color = PongGame::LEFT_COLOR;
        paddle_left_pipeline_instance.setUniform("material", &color);
        color = PongGame::RIGHT_COLOR;
        paddle_right_pipeline_instance.setUniform("material", &color);


        AudioClipInfo audio_clip_info{};
        audio_clip_info.path = "sounds/8BitHit.wav";
        bounce_sound.alloc(audio_clip_info);

        AudioClipInstanceInfo audio_clip_instance_info{};
        audio_clip_instance_info.clip = &bounce_sound;
        audio_clip_instance_info.volume = 0.1;
        audio_clip_instance_info.pitch = 1.0f;
        bounce_sound_instance.alloc(audio_clip_instance_info);
    }

    void PongGameScene::OnWindowSizeChange(Window *window) {
        render_target.setResolution(window->getSize());
    }

    void PongGameScene::onRenderTargetResolutionChange(RasterizationTarget *render_target) {
        Entity camera_entity = getCameraEntity();
        Camera2D *camera = camera_entity.get<Camera2D>();

        float height = camera->getZoomRatio();
        float width = height * camera->getAspectRatio();

        game_area = Area{
            {-width / 2.0f, -height / 2.0f},
            {width, height}
        };

        vec3 paddle_left_position = paddle_left_entity.get<Transform>()->position();
        vec3 paddle_right_position = paddle_right_entity.get<Transform>()->position();
        paddle_left_position.x = -game_area.size.x / 2 + 1;
        paddle_right_position.x = game_area.size.x / 2 - 1;
        paddle_left_entity.get<Transform>()->setPosition(paddle_left_position);
        paddle_right_entity.get<Transform>()->setPosition(paddle_right_position);
    }
}
