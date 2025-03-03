#include "MenuScene.h"

void MenuScene::createResources()
{
	MeshInfo mesh_info{};
	mesh_info.attribute_info_count = 1;
	mesh_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION2D;
	mesh_info.flags = MESH_FLAG_NONE;
	rounded_rectangle_mesh = Resources::createMesh(mesh_info);
	//float vertices[] = {
	//	-0.5f, -0.5f,
	//	0.5f, -0.5f,
	//	0.0f, 0.5f,
	//};
	//rounded_rectangle_mesh->setBuffer(0, vertices, 3);
	Geometry::createRoundedRectTriangleFan(*rounded_rectangle_mesh, 10.0, 5.0, 0.2, 6, VERTEX_FLAG_NONE, PIVOT_CENTER);

	ShaderInfo shader_info{};
	shader_info.path = "shaders/defaults/Position2D.vert";
	shader_info.stage = SHADER_STAGE_VERTEX;
	vertex_shader = Resources::createShader(shader_info);

	shader_info.path = "shaders/defaults/Position.frag";
	shader_info.stage = SHADER_STAGE_FRAGMENT;
	fragment_shader = Resources::createShader(shader_info);


	RasterizationPipelineInfo pipeline_info{};
	pipeline_info.attribute_info_count = 1;
	pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION2D;
	pipeline_info.vertex_shader = vertex_shader;
	pipeline_info.fragment_shader = fragment_shader;
	pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_NONE;
	pipeline_info.topology = VERTEX_TOPOLOGY_TRIANGLE_FAN;

	fan_pipeline = Resources::createRasterizationPipeline(pipeline_info);

	RasterizationPipelineInstanceInfo pipeline_instance_info{};
	pipeline_instance_info.flags = RASTERIZATION_PIPELINE_INSTANCE_FLAG_NONE;
	pipeline_instance_info.rasterization_pipeline = fan_pipeline;

	fan_pipeline_instance = Resources::createRasterizationPipelineInstance({fan_pipeline});
	vec4 material = vec4(1.0, 0.0, 0.0, 1.0);
	fan_pipeline_instance->setUniform("material", &material);
}

void MenuScene::setupScene()
{
	Entity camera_entity = createEntity3D();
	camera_entity.attach<Camera2D>();
	setCameraEntity(camera_entity);

	Entity rounded_rectangle_entity = createEntity3D();

	MeshRenderer* rounded_rectangle_renderer = rounded_rectangle_entity.attach<MeshRenderer>();
	rounded_rectangle_renderer->mesh = rounded_rectangle_mesh;
	rounded_rectangle_renderer->pipeline_instance = fan_pipeline_instance;
}

MenuScene::MenuScene()
{
	createResources();
	setupScene();
}

MenuScene::~MenuScene()
{
	delete rounded_rectangle_mesh;
	delete vertex_shader;
	delete fragment_shader;
	delete fan_pipeline;
	delete fan_pipeline_instance;
}
