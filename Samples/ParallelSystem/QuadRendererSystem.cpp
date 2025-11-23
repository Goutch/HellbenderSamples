//
// Created by username on 3/21/2025.
//

#include "QuadRendererSystem.h"
#include "HBE.h"

QuadRendererSystem::QuadRendererSystem(Scene* scene): System(scene)
{
	MeshInfo triangle_info{};
	triangle_info.attribute_info_count = 1;
	triangle_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;

	quad_mesh = Resources::createMesh(triangle_info);

	Geometry::createQuad(*quad_mesh, 1, 1, VERTEX_FLAG_NONE, PIVOT_CENTER);

	ShaderInfo vertex_shader_info{};
	ShaderInfo fragment_shader_info{};

	vertex_shader_info.path = "shaders/Colored.vert";
	vertex_shader_info.stage = SHADER_STAGE_VERTEX;

	fragment_shader_info.path = "shaders/Colored.frag";
	fragment_shader_info.stage = SHADER_STAGE_FRAGMENT;

	vertex_shader = Resources::createShader(vertex_shader_info);
	fragment_shader = Resources::createShader(fragment_shader_info);

	RasterizationPipelineInfo pipeline_info{};
	pipeline_info.attribute_info_count = 1;
	pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
	pipeline_info.vertex_shader = vertex_shader;
	pipeline_info.fragment_shader = fragment_shader;
	pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_NONE;

	pipeline = Resources::createRasterizationPipeline(pipeline_info);

	RasterizationPipelineInstanceInfo pipeline_instance_info{};
	pipeline_instance_info.rasterization_pipeline = pipeline;
	pipeline_instance_info.flags = RASTERIZATION_PIPELINE_INSTANCE_FLAG_NONE;

	pipeline_instance = Resources::createRasterizationPipelineInstance(pipeline_instance_info);

	scene->onDraw.subscribe(this, &QuadRendererSystem::draw);
	scene->onUpdate.subscribe(this, &QuadRendererSystem::update);
}

QuadRendererSystem::~QuadRendererSystem()
{
	delete vertex_shader;
	delete fragment_shader;
	delete pipeline;
	delete pipeline_instance;
}

void QuadRendererSystem::update(float delta)
{
	if (Input::getKeyDown(KEY_R))
	{
		active = !active;
	}
}

void QuadRendererSystem::draw(RenderGraph* render_graph)
{
	if (!active)
		return;
	auto group = scene->group<Transform, QuadRenderer>();
	int i = 0;
	float push_constants_data[20];
	PushConstantInfo push_constants;
	push_constants.name = "constants";
	push_constants.data = &push_constants_data;
	push_constants.size = sizeof(mat4) + sizeof(vec4);
	for (auto [handle, transform, quad_renderer] : group)
	{
		push_constants.data = &push_constants_data;
		memcpy(&push_constants_data[0], &transform.local(), sizeof(mat4));
		memcpy(&push_constants_data[16], &quad_renderer.color, sizeof(vec4));

		DrawCmdInfo cmd{};
		cmd.mesh = quad_mesh;
		cmd.flags = DRAW_CMD_FLAG_ORDERED;
		cmd.push_constants_count = 1;
		cmd.push_constants = &push_constants;
		cmd.order_in_layer = i;
		cmd.pipeline_instance = pipeline_instance;
		i++;
		render_graph->add(cmd);
	}
}
