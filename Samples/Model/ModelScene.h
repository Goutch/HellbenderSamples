#pragma once

#include "HBE.h"

using namespace HBE;


class ModelScene : public Scene
{
	Shader frag;
	Shader vert;
	RasterizationPipeline model_pipeline_2_sided;
	RasterizationPipeline model_pipeline;
	Model sponza_model;
public:
	ModelScene()
	{
		//-------------------RESOURCES CREATION--------------------------------------
		frag.loadGLSL("/shaders/defaults/PositionUVNormalTextured.frag",SHADER_STAGE_FRAGMENT);
		vert.loadGLSL( "/shaders/defaults/PositionUVNormal.vert",SHADER_STAGE_VERTEX);

		std::vector<VertexAttributeInfo> attribute_infos;
		//vertex binding
		attribute_infos.emplace_back();
		attribute_infos[0].location = 0;
		attribute_infos[0].size = sizeof(vec3);
		attribute_infos[0].flags = VERTEX_ATTRIBUTE_FLAG_NONE;

		attribute_infos.emplace_back();
		attribute_infos[1].location = 1;
		attribute_infos[1].size = sizeof(vec2);
		attribute_infos[1].flags = VERTEX_ATTRIBUTE_FLAG_NONE;

		attribute_infos.emplace_back();
		attribute_infos[2].location = 2;
		attribute_infos[2].size = sizeof(vec3);
		attribute_infos[2].flags = VERTEX_ATTRIBUTE_FLAG_NONE;

		RasterizationPipelineInfo pipeline_info{};
		pipeline_info.attribute_infos = attribute_infos.data();
		pipeline_info.attribute_info_count = attribute_infos.size();
		pipeline_info.fragment_shader = frag.getHandle();
		pipeline_info.vertex_shader = vert.getHandle();

		pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_FRONT_COUNTER_CLOCKWISE | //gltf primitives are counterclockwise
			RASTERIZATION_PIPELINE_FLAG_ALLOW_EMPTY_DESCRIPTOR;
		model_pipeline_2_sided.alloc(pipeline_info);
		pipeline_info.flags |= RASTERIZATION_PIPELINE_FLAG_CULL_BACK;
		model_pipeline.alloc(pipeline_info);

		DefaultModelParserInfo parser_info{};
		parser_info.rasterization_pipeline = model_pipeline.getHandle();
		parser_info.rasterization_pipeline_2_sided = model_pipeline_2_sided.getHandle();
		parser_info.texture_names.emplace(MODEL_TEXTURE_TYPE_ALBEDO, "albedo");
		parser_info.material_property_name = "material";
		DefaultModelParser parser = DefaultModelParser(parser_info);

		ModelInfo model_info{};
		model_info.flags = MODEL_FLAG_NONE;
		model_info.parser = &parser;

		Log::debug("Loading sponza model");
		model_info.path = "/models/sponza/Sponza.gltf";
		sponza_model.load(model_info);

		//-------------------SCENE CREATION--------------------------------------

		//Create camera
		Entity camera_entity = createEntity3D();
		Camera* camera = camera_entity.attach<Camera>();
		CameraController* camera_controller = camera_entity.attach<CameraController>();
		camera_entity.get<Transform>()->translate(vec3(0, 2, 0));
		setCameraEntity(camera_entity);

		auto sponza = createEntity3D();
		ModelRenderer* sponza_renderer = sponza.attach<ModelRenderer>();
		sponza_renderer->model = &sponza_model;
		sponza.get<Transform>()->setLocalScale(vec3(1));
		sponza.get<Transform>()->translate(vec3(0, 0, 0));
	}
};
