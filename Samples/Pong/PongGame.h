#pragma once

#include "HBE.h"
#include "PongGameScene.h"
#include "PongUIScene.h"

using namespace HBE;

namespace Pong {
	struct PongGameState {
		uint32 score_left;
		uint32 score_right;
	};

	class PongGame {
	public:
		const static vec4 LEFT_COLOR;
		const static vec4 RIGHT_COLOR;
	private:
		PongUIScene *ui_scene = nullptr;
		PongGameScene *game_scene = nullptr;
		PongGameState game_state;
		event_subscription_id present_subscription_id;

	public :
		PongGame();

		~PongGame();

	private:
		void onPresent();


	};
}

