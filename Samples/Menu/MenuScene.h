#pragma once
#include "HBE.h"

class MenuScene : public Scene
{
	Mesh* rounded_rectangle_mesh = nullptr;
	Mesh* rounded_rectangle_outline_mesh = nullptr;
	RasterizationPipeline* fan_pipeline = nullptr;
	RasterizationPipeline* strip_pipeline = nullptr;
	RasterizationPipelineInstance* fan_pipeline_instance = nullptr;
	RasterizationPipelineInstance* strip_pipeline_instance = nullptr;
	Shader* vertex_shader = nullptr;
	Shader* fragment_shader = nullptr;

public:
	void createResources();
	void setupScene();
	MenuScene();
	~MenuScene() override;
};
