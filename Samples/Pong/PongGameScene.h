#pragma once

#include "HBE.h"

using namespace HBE;
namespace Pong {
	struct Area {
		vec2 position;
		vec2 size;
	};

	struct PongGameState;

	class PongGameScene : public Scene {
		Area game_area;

		Mesh *quad_mesh;
		Shader *vertex_shader;
		Shader *fragment_shader;
		RasterizationPipeline *pipeline;
		RasterizationPipelineInstance *paddle_left_pipeline_instance;
		RasterizationPipelineInstance *paddle_right_pipeline_instance;
		Entity paddle_left_entity;
		Entity paddle_right_entity;
		RasterizationTarget *render_target;
		AudioClip *bounce_sound;
		AudioClipInstance *bounce_sound_instance;

		event_subscription_id on_window_size_change_subscription_id;
	public:
		PongGameScene(PongGameState &game_state);

		~PongGameScene() override;

		void OnWindowSizeChange(Window *window);

		void setupScene();

		void createResources();

		Area &getArea();

		void onRenderTargetResolutionChange(RasterizationTarget *render_target);

		Entity createPaddle(vec3 position, KEY up_key, KEY down_key, RasterizationPipelineInstance *paddle_pipeline_instance);


	public:
		Entity createBall(vec2 position, vec2 velocity);

	};
}
