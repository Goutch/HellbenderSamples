#pragma once

#include "HBE.h"

struct QuadRenderer {
	COMPONENT_IDS(QuadRenderer)
	vec4 color = vec4(1);
};

class QuadRendererSystem : System {
	Mesh *quad_mesh;

	Shader *vertex_shader;
	Shader *fragment_shader;
	RasterizationPipeline *pipeline;
	RasterizationPipelineInstance *pipeline_instance;
	bool active = true;
	event_subscription_id draw_subscription_id;
	event_subscription_id update_subscription_id;
public:
	QuadRendererSystem(Scene *scene);

	~QuadRendererSystem();

	void update(float delta);

	void draw(RenderGraph *render_graph);
};
