//
// Created by username on 3/21/2025.
//

#include "QuadRendererSystem.h"
#include "HBE.h"

QuadRendererSystem::QuadRendererSystem(Scene *scene) : System(scene) {
	MeshInfo quad_info{};
	quad_info.attribute_info_count = 1;
	quad_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;

	quad_mesh.alloc(quad_info);

	Geometry::createQuad(quad_mesh, 1, 1, VERTEX_FLAG_NONE, PIVOT_CENTER);

	vertex_shader.loadGLSL("shaders/Colored.vert", SHADER_STAGE_VERTEX);
	fragment_shader.loadGLSL("shaders/Colored.frag", SHADER_STAGE_FRAGMENT);

	RasterizationPipelineInfo pipeline_info{};
	pipeline_info.attribute_info_count = 1;
	pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
	pipeline_info.vertex_shader = vertex_shader.getHandle();
	pipeline_info.fragment_shader = fragment_shader.getHandle();
	pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_NONE;

	pipeline.alloc(pipeline_info);
	pipeline.allocInstance(pipeline_instance);

	scene->onDraw.subscribe(draw_subscription_id, this, &QuadRendererSystem::draw);
	scene->onUpdate.subscribe(update_subscription_id, this, &QuadRendererSystem::update);
}

QuadRendererSystem::~QuadRendererSystem() {
	scene->onDraw.unsubscribe(draw_subscription_id);
	scene->onUpdate.unsubscribe(update_subscription_id);
}

void QuadRendererSystem::update(float delta) {
	if (input.getKeyDown(KEY_R)) {
		active = !active;
	}
}

void QuadRendererSystem::draw(RenderGraph *render_graph) {
	if (!active)
		return;
	auto group = scene->group<Transform, QuadRenderer>();
	int i = 0;
	float push_constants_data[20];
	PushConstantInfo push_constants;
	push_constants.name = "constants";
	push_constants.data = &push_constants_data;
	push_constants.size = sizeof(mat4) + sizeof(vec4);
	for (auto [handle, transform, quad_renderer]: group) {
		push_constants.data = &push_constants_data;
		memcpy(&push_constants_data[0], &transform.local(), sizeof(mat4));
		memcpy(&push_constants_data[16], &quad_renderer.color, sizeof(vec4));

		DrawCmdInfo cmd{};
		cmd.mesh = quad_mesh.getHandle();
		cmd.flags = DRAW_CMD_FLAG_ORDERED;
		cmd.push_constants_count = 1;
		cmd.push_constants = &push_constants;
		cmd.order_in_layer = i;
		cmd.pipeline_instance_handle = pipeline_instance.getHandle();
		cmd.rasterization_pipeline_handle = pipeline.getHandle();
		i++;
		render_graph->add(cmd);
	}
}
