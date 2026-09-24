#pragma once

#include "HBE.h"

class MenuScene : public Scene {
	Mesh rounded_rectangle_mesh;
	Mesh rounded_rectangle_outline_mesh;
	RasterizationPipeline fan_pipeline;
	RasterizationPipeline strip_pipeline;
	PipelineInstance fan_pipeline_instance;
	PipelineInstance strip_pipeline_instance;
	Shader vertex_shader;
	Shader fragment_shader;

public:
	MenuScene() {
		MeshInfo mesh_info{};
		mesh_info.attribute_info_count = 1;
		mesh_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION2D;
		mesh_info.flags = MESH_FLAG_NONE;
		rounded_rectangle_mesh.alloc(mesh_info);
		rounded_rectangle_outline_mesh.alloc(mesh_info);
		Geometry::createRoundedRectTriangleFan(rounded_rectangle_mesh, 10.0, 5.0, 0.5, 10, VERTEX_FLAG_NONE, PIVOT_CENTER);
		Geometry::createRoundedRectOutlineTriangleStrip(rounded_rectangle_outline_mesh, 10.0, 5.0, 0.5, 0.2f, 10, VERTEX_FLAG_NONE, PIVOT_CENTER);

		vertex_shader.loadGLSL("shaders/defaults/Position2D.vert", SHADER_STAGE_VERTEX);
		fragment_shader.loadGLSL("shaders/defaults/Position.frag", SHADER_STAGE_FRAGMENT);

		RasterizationPipelineInfo pipeline_info{};
		pipeline_info.attribute_info_count = 1;
		pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION2D;
		pipeline_info.vertex_shader = vertex_shader.getHandle();
		pipeline_info.fragment_shader = fragment_shader.getHandle();
		pipeline_info.topology = VERTEX_TOPOLOGY_TRIANGLE_FAN;
		pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_NO_DEPTH_TEST;

		fan_pipeline.alloc(pipeline_info);
		fan_pipeline.allocInstance(fan_pipeline_instance);
		pipeline_info.topology = VERTEX_TOPOLOGY_TRIANGLE_STRIP;

		strip_pipeline.alloc(pipeline_info);
		strip_pipeline.allocInstance(strip_pipeline_instance);

		vec4 material = vec4(0.02, 0.02, 0.02, 1.0);
		fan_pipeline_instance.setUniform("material", &material);
		material = vec4(1.0, 1.0, 1.0, 1.0);
		strip_pipeline_instance.setUniform("material", &material);


		Entity camera_entity = createEntity3D();
		camera_entity.attach<Camera2D>();
		setCameraEntity(camera_entity);

		Entity rounded_rectangle_outline_entity = createEntity3D();
		Entity rounded_rectangle_entity = createEntity3D();

		MeshRenderer *rounded_rectangle_renderer = rounded_rectangle_entity.attach<MeshRenderer>();
		rounded_rectangle_renderer->mesh = rounded_rectangle_mesh.getHandle();
		rounded_rectangle_renderer->pipeline_instance = fan_pipeline_instance.getHandle();
		rounded_rectangle_renderer->ordered = true;

		MeshRenderer *rounded_rectangle_outline_renderer = rounded_rectangle_outline_entity.attach<MeshRenderer>();
		rounded_rectangle_outline_renderer->mesh = rounded_rectangle_outline_mesh.getHandle();
		rounded_rectangle_outline_renderer->pipeline_instance = strip_pipeline_instance.getHandle();
		rounded_rectangle_outline_renderer->ordered = true;

		setParent(rounded_rectangle_outline_entity, rounded_rectangle_entity);
	}
};
