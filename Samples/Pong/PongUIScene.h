#pragma once
#include "FPSCounter.h"
#include "HBE.h"


using namespace HBE;

namespace Pong {
	struct PongGameState;

	class PongUIScene : public Scene {
		const float SCORE_TEXT_SIZE = 50.0f;
		FPSCounter *fps_counter;
		RasterizationTarget *render_target;
		PongGameState *game_state;

		Font *font;
		RasterizationPipeline *pipeline;

		RasterizationPipelineInstance *left_text_pipeline_instance;
		RasterizationPipelineInstance *right_text_pipeline_instance;

		Mesh *score_left_mesh;
		Mesh *score_right_mesh;

		Entity score_left_entity;
		Entity score_right_entity;

		uint32_t last_score_left = 0;
		uint32_t last_score_right = 0;
	public:
		void updateUI(float delta);
		PongUIScene(PongGameState &game_state);

		~PongUIScene() override;

		void OnWindowSizeChange(Window *window);

		Entity createScore(RasterizationPipelineInstance *pipeline_instance, Mesh *text);

		void createResources();
		void setupScene();
	};
}

