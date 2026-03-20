#pragma once

#include "HBE.h"

using namespace HBE;

class TextScene : public Scene {
	std::string text_str = "Hello world!\n"
	                       "This is a test of the text rendering system\n"
	                       "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{};':\",./<>?\\|`~\n"
	                       "You can try to type something now...or zoom in/out\n";

	Shader text_vertex_shader;
	Shader text_fragment_shader;
	Mesh text_mesh;
	RasterizationPipeline text_pipeline;
	PipelineInstance text_pipeline_instance;
	Font font;
	event_subscription_id on_char_down_subscription_id;

	void onCharacterInput(char codepoint) {
		text_str += codepoint;
		vec2 size;
		Geometry::updateText(context,text_mesh.getHandleRef(),
		                     text_str,
		                     font,
		                     1.0,
		                     0.5,
		                     TEXT_ALIGNMENT_CENTER,
		                     PIVOT_CENTER,
		                     size);

	}

	void update(float delta) {
		Scene::update(delta);

		if (input.getKeyDown(KEY_ENTER)) {
			onCharacterInput(static_cast<char>('\n'));
		}
		if (input.getKeyDown(KEY_BACKSPACE)) {
			text_str.pop_back();
			vec2 size;
			Geometry::updateText(context,text_mesh.getHandleRef(),
			                     text_str,
			                     font,
			                     1.0,
			                     0.5,
			                     TEXT_ALIGNMENT_CENTER,
			                     PIVOT_CENTER,
			                     size);
		}
		if (input.getKey(KEY_LEFT)) {
			getCameraEntity().get<Transform>()->translate(vec3(-1 * delta, 0, 0));
		}
		if (input.getKey(KEY_RIGHT)) {
			getCameraEntity().get<Transform>()->translate(vec3(1 * delta, 0, 0));
		}
		if (input.getKey(KEY_UP)) {
			getCameraEntity().get<Transform>()->translate(vec3(0, 1 * delta, 0));
		}
		if (input.getKey(KEY_DOWN)) {
			getCameraEntity().get<Transform>()->translate(vec3(0, -1 * delta, 0));
		}

		getCameraEntity().get<Camera2D>()->setZoomRatio(getCameraEntity().get<Camera2D>()->getZoomRatio() + input.getMouseWheelInput());
	}

private:
	void createResources() {

		text_vertex_shader.loadGLSL("shaders/defaults/TextMSDF.vert",SHADER_STAGE_VERTEX);
		text_fragment_shader.loadGLSL("shaders/defaults/TextMSDF.frag",SHADER_STAGE_FRAGMENT);

		RasterizationPipelineInfo pipeline_info{};
		pipeline_info.attribute_info_count = 1;

		pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
		pipeline_info.vertex_shader = text_vertex_shader.getHandle();
		pipeline_info.fragment_shader = text_fragment_shader.getHandle();
		pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_NO_DEPTH_TEST;
		text_pipeline.alloc(pipeline_info);
		text_pipeline.allocInstance(text_pipeline_instance);

		std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{};':\",./<>?\\|`~";
		FontInfo font_info{};
		font_info.path = "fonts/Roboto-Regular.ttf";
		font_info.characters = characters.data();
		font_info.characters_count = characters.size();
		font_info.glyph_resolution = 64;

		font.load(font_info);

		text_pipeline_instance.setImage("mtsdf", font.getTextureAtlas()->getHandle());
		vec2 size;
		Geometry::updateText(context,text_mesh.getHandleRef(),text_str,
		                                 font,
		                                 1.0,
		                                 0.5,
		                                 TEXT_ALIGNMENT_CENTER,
		                                 PIVOT_CENTER,
		                                 size);

		vec4 color = vec4(1, 1, 1, 1);
		text_pipeline_instance.setUniform("material", &color);
	}

	void setupScene() {
		Entity camera_entity = createEntity3D();

		Camera2D *camera2D = camera_entity.attach<Camera2D>();
		camera2D->setZoomRatio(35.0f);

		Entity text_entity = createEntity3D();
		MeshRenderer *text_renderer = text_entity.attach<MeshRenderer>();
		text_renderer->ordered = true;
		text_renderer->layer = 0;
		text_renderer->pipeline_instance = text_pipeline_instance.getHandle();
		text_renderer->mesh = text_mesh.getHandle();
	}

public:
	TextScene() {
		createResources();
		setupScene();
		input.onCharDown.subscribe(on_char_down_subscription_id, this, &TextScene::onCharacterInput);
	}

	~TextScene() {
		input.onCharDown.unsubscribe(on_char_down_subscription_id);
	}


};