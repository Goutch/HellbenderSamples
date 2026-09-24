#pragma once

#include "HBE.h"

using namespace HBE;

class OrderedRenderingScene : public Scene {
	Mesh triangle_mesh;

	Shader vertex_shader;
	Shader fragment_shader;
	RasterizationPipeline pipeline;
	std::vector<PipelineInstance> pipeline_instances;


public:

	OrderedRenderingScene() {
		createResources();
		setupScene();
	}

	Entity createTriangle(vec3 position, vec4 color) {
		PipelineInstance& pipeline_instance = pipeline_instances.emplace_back();
		pipeline.allocInstance(pipeline_instance);
		pipeline_instance.setUniform("material", &color);

		Entity triangle_entity = createEntity3D();
		MeshRenderer *triangle_renderer = triangle_entity.attach<MeshRenderer>();
		triangle_renderer->mesh = triangle_mesh.getHandle();
		triangle_renderer->pipeline_instance = pipeline_instance.getHandle();
		triangle_renderer->ordered = true;
		triangle_entity.get<Transform>()->setPosition(position);

		return triangle_entity;

	}


	void setupScene() {
		Entity camera_entity = createEntity3D();
		Camera2D *camera = camera_entity.attach<Camera2D>();
		setCameraEntity(camera_entity);

		std::vector<Entity> entities;
		int n = 50;
		for (int i = 0; i < n; ++i) {
			Entity triangle = createTriangle(vec3(0.2, 0, 0),
			                                 vec4(Random::floatRange(0, 1),
			                                      Random::floatRange(0, 1),
			                                      Random::floatRange(0, 1),
			                                      1));

			entities.push_back(triangle);
		}
		entities[n-1].get<Transform>()->setPosition(vec3(-5, 0, 0));
		for (int i = n - 1; i >= 1; --i) {
			setParent(entities[i - 1], entities[i]);
		}
	}

	void createResources() {
		MeshInfo triangle_info{};
		triangle_info.attribute_info_count = 1;
		triangle_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;

		triangle_mesh.alloc(triangle_info);

		float vertices[] = {
				-0.5f, -0.5f, 0.0f,
				0.5f, -0.5f, 0.0f,
				0.0f, 0.5f, 0.0f
		};
		triangle_mesh.setBuffer(0, vertices, 3);

		vertex_shader.loadGLSL("shaders/defaults/Position.vert",SHADER_STAGE_VERTEX);
		fragment_shader.loadGLSL("shaders/defaults/Position.frag",SHADER_STAGE_FRAGMENT);

		RendererResources resources;
		context.rendererGetResources(resources);

		RasterizationPipelineInfo pipeline_info{};
		pipeline_info.attribute_info_count = 1;
		pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
		pipeline_info.vertex_shader = vertex_shader.getHandle();
		pipeline_info.fragment_shader = fragment_shader.getHandle();
		pipeline_info.rasterization_target = resources.main_render_target;
		pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_NO_DEPTH_TEST;

		pipeline.alloc(pipeline_info);
	}

};