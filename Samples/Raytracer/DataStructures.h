
#pragma once

#include "HBE.h"
#include "core/resource/raytracing/AABBAccelerationStructure.h"

struct MaterialData {
	vec4 albedo;
	vec4 emission;
	int albedo_texture_index = -1;
	int normal_texture_index = -1;
	alignas(8) float roughness;
};
struct InstanceInfo {
	uint32_t material_index;
	uint32_t mesh_index;
	uint32_t indices_size;
};

struct CameraProperties {
	mat4 transform;
	mat4 projection;
};

struct Frame {
	float time = 0;
	uint32_t index = 0;
	uint32_t sample_count = 1;
	uint32_t max_bounces = 3;
	float scattering_multiplier = 12.0f;
	float density_falloff = 10.0f;
	float exposure = 1.0f;
	float gamma = 2.2f;
	uint use_blue_noise = 1;
};

struct GBufferResources {
	std::vector<CameraProperties> history_camera;
	//raytracer
	std::vector<ImageHandle> history_albedo;
	std::vector<ImageHandle> history_normal_depth;
	std::vector<ImageHandle> history_motion;
	std::vector<ImageHandle> history_irradiance;
	std::vector<ImageHandle> history_position;
	//denoiser
	Image denoiser_temporal_accumulation_texture;
	Image denoiser_irradiance_vertical_blur_texture;
};

struct RaytracerResources {
	PipelineInstance pipeline_instance;
	RaytracingPipeline pipeline;
	Shader raygen_shader;
	Shader miss_shader;
	Shader closest_hit_aabb;
	Shader closest_hit_mesh;
	Shader any_hit_alpha_clip;
	Shader intersection_box;
	Shader intersection_sphere;

	std::vector<ShaderHandle> miss_shader_handles;
	std::vector<ShaderHandle> hit_shader_handles;
	std::vector<RaytracingShaderGroup> shader_groups;
	std::vector<ImageHandle> st_blue_noise;

};

struct SceneResources {
	RootAccelerationStructure root_acceleration_structure;
	AABBAccelerationStructure aabb_acceleration_structure;
	std::vector<AccelerationStructureInstance> acceleration_structure_instances;
	std::vector<InstanceInfo> instances;
	StorageBuffer instance_buffer;

	std::vector<MaterialData> materials;
	StorageBuffer material_buffer;

	//deleted in model parser
	std::vector<MeshHandle> meshes;
	std::vector<BufferHandle> normals;
	std::vector<BufferHandle> indices;
	std::vector<BufferHandle> uvs;
	std::vector<ImageHandle > textures;
	std::vector<MeshAccelerationStructureHandle > mesh_acceleration_structures;
};

struct DenoisingResources {
	Shader temporal_accumulation_shader;
	ComputePipeline temporal_accumulation_pipeline;
	PipelineInstance temporal_accumulation_instance;

	Shader vertical_blur_shader;
	ComputePipeline vertical_blur_pipeline;
	PipelineInstance vertical_blur_instance;

	Shader horizontal_blur_shader;
	ComputePipeline horizontal_blur_pipeline;
	PipelineInstance horizontal_blur_instance;

};