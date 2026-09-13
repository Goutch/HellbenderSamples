#include "Raytracer.h"
#include "RaytracingScene.h"

Raytracer::Raytracer(uint32_t history_count) :  context(*Application::instance->getContext()) {
	createPrimaryRaytracingResources(history_count);

	raytracing_resources.pipeline_instance.setImageArray("blue_noise", raytracing_resources.st_blue_noise.data(), raytracing_resources.st_blue_noise.size());
}


Raytracer::~Raytracer() {
	raytracing_resources.miss_shader_handles.clear();
	raytracing_resources.hit_shader_handles.clear();
	raytracing_resources.st_blue_noise.clear();
}

void Raytracer::traceRays(Frame &frame, GBufferResources &gbuffer_resources, RootAccelerationStructure *root_acceleration_structure) {
	raytracing_resources.pipeline_instance.setUniform("frame", &frame);
	raytracing_resources.pipeline_instance.setUniform("camera_history", gbuffer_resources.history_camera.data());
	uvec3 resolution;
	context.getImageSize(gbuffer_resources.history_albedo[0],resolution);
	TraceRaysCmdInfo trace_rays_cmd_info{};
	trace_rays_cmd_info.pipeline_instance = raytracing_resources.pipeline_instance.getHandle();
	trace_rays_cmd_info.resolution = resolution;
	context.cmdTraceRays(trace_rays_cmd_info);
}

void Raytracer::setGBufferUniforms(GBufferResources &gbuffer_resources) {
	raytracing_resources.pipeline_instance.setImageArray("historyAlbedo", gbuffer_resources.history_albedo.data(), gbuffer_resources.history_albedo.size());
	raytracing_resources.pipeline_instance.setImageArray("historyNormalDepth", gbuffer_resources.history_normal_depth.data(),gbuffer_resources.history_normal_depth.size());
	raytracing_resources.pipeline_instance.setImageArray("historyMotion", gbuffer_resources.history_motion.data(), gbuffer_resources.history_motion.size());
	raytracing_resources.pipeline_instance.setImageArray("historyIrradiance", gbuffer_resources.history_irradiance.data(), gbuffer_resources.history_irradiance.size());
	raytracing_resources.pipeline_instance.setImageArray("historyPosition", gbuffer_resources.history_position.data(), gbuffer_resources.history_position.size());
}

RaytracerResources &Raytracer::getRaytracingResources() {
	return raytracing_resources;
}

void Raytracer::setSceneUniforms(SceneResources &scene_resources) {
	if (scene_resources.textures.size() > 0)
		raytracing_resources.pipeline_instance.setImageArray("textures", scene_resources.textures.data(), scene_resources.textures.size());
	raytracing_resources.pipeline_instance.setAccelerationStructure("topLevelAS", scene_resources.root_acceleration_structure.getHandle());
	raytracing_resources.pipeline_instance.setStorageBuffer("instances", scene_resources.instance_buffer.getHandle(), scene_resources.instance_buffer.getCount(), 0);
	raytracing_resources.pipeline_instance.setStorageBufferArray("mesh_indices_buffers", scene_resources.indices.data(), scene_resources.indices.size());
	raytracing_resources.pipeline_instance.setStorageBufferArray("mesh_normals_buffers", scene_resources.normals.data(), scene_resources.normals.size());
	raytracing_resources.pipeline_instance.setStorageBufferArray("mesh_tex_coords_buffers", scene_resources.uvs.data(), scene_resources.uvs.size());
	raytracing_resources.pipeline_instance.setStorageBuffer("materials", scene_resources.material_buffer.getHandle(), scene_resources.material_buffer.getCount(),0);
}


void Raytracer::createPrimaryRaytracingResources(uint32_t history_count) {
	std::string preamble = "#define HISTORY_COUNT " + std::to_string(history_count) + "\n";
	raytracing_resources.raygen_shader.loadGLSL("shaders/raytracing/raygen/raygen.glsl",
												SHADER_STAGE_RAY_GEN,
												preamble.c_str());
	raytracing_resources.miss_shader.loadGLSL("shaders/raytracing/miss/primary_miss.glsl",
											  SHADER_STAGE_RAY_MISS,
											  preamble.c_str());
	raytracing_resources.closest_hit_aabb.loadGLSL("shaders/raytracing/closestHit/primary_closest_hit_aabb.glsl",
											  SHADER_STAGE_CLOSEST_HIT,
											  preamble.c_str());
	raytracing_resources.closest_hit_mesh.loadGLSL("shaders/raytracing/closestHit/primary_closest_hit_mesh.glsl",
											  SHADER_STAGE_CLOSEST_HIT,
											  preamble.c_str());
	raytracing_resources.any_hit_alpha_clip.loadGLSL("shaders/raytracing/anyHit/alpha_clipping_anyhit.glsl",
											  SHADER_STAGE_ANY_HIT,
											  preamble.c_str());
	raytracing_resources.intersection_box.loadGLSL("shaders/raytracing/intersect/intersect_box.glsl",
											  SHADER_STAGE_INTERSECTION,
											  preamble.c_str());
	raytracing_resources.intersection_sphere.loadGLSL("shaders/raytracing/intersect/intersect_sphere.glsl",
											  SHADER_STAGE_INTERSECTION,
											  preamble.c_str());

	raytracing_resources.miss_shader_handles.push_back(raytracing_resources.miss_shader.getHandle());
	raytracing_resources.hit_shader_handles.push_back(raytracing_resources.closest_hit_aabb.getHandle());
	raytracing_resources.hit_shader_handles.push_back(raytracing_resources.closest_hit_mesh.getHandle());
	raytracing_resources.hit_shader_handles.push_back(raytracing_resources.any_hit_alpha_clip.getHandle());
	raytracing_resources.hit_shader_handles.push_back(raytracing_resources.intersection_box.getHandle());
	raytracing_resources.hit_shader_handles.push_back(raytracing_resources.intersection_sphere.getHandle());

	//primary rays
	raytracing_resources.shader_groups.push_back({0, -1, 3}); //box
	raytracing_resources.shader_groups.push_back({0, -1, 4}); //sphere
	raytracing_resources.shader_groups.push_back({1, 2, -1}); //Mesh

	RaytracingPipelineInfo primary_raytraycing_pipeline_info{};

	primary_raytraycing_pipeline_info.raygen_shader = raytracing_resources.raygen_shader.getHandle();
	primary_raytraycing_pipeline_info.miss_shaders = raytracing_resources.miss_shader_handles.data();
	primary_raytraycing_pipeline_info.miss_shader_count = raytracing_resources.miss_shader_handles.size();
	primary_raytraycing_pipeline_info.hit_shaders = raytracing_resources.hit_shader_handles.data();
	primary_raytraycing_pipeline_info.hit_shader_count = raytracing_resources.hit_shader_handles.size();

	primary_raytraycing_pipeline_info.shader_group_count = raytracing_resources.shader_groups.size();
	primary_raytraycing_pipeline_info.shader_groups = raytracing_resources.shader_groups.data();
	primary_raytraycing_pipeline_info.max_recursion_depth = 1;

	raytracing_resources.pipeline.alloc(primary_raytraycing_pipeline_info);
	raytracing_resources.pipeline.allocInstance(raytracing_resources.pipeline_instance);

	std::string blue_noise_path = "textures/stbn_unitvec3_cosine_2Dx1D_128x128x64_";
	for (uint32_t i = 0; i < 64; i++) {
		raytracing_resources.st_blue_noise.push_back(
				Image::load(
						(blue_noise_path + std::to_string(i) + ".png").c_str(),
						IMAGE_FORMAT_RGBA8_UNORM)
				);
	}
}
