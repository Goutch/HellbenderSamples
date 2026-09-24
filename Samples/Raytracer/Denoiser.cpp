
#include "Denoiser.h"

void Denoiser::accumulate(Frame &frame, GBufferResources &gbuffer_resources) {
	denoiser_resources.temporal_accumulation_instance.setUniform("frame", &frame);
	denoiser_resources.temporal_accumulation_instance.setUniform("camera_history", gbuffer_resources.history_camera.data());
	uvec3 size;
	context.getImageSize(gbuffer_resources.history_albedo[0],size);
	ComputeDispatchCmdInfo compute_dispatch_cmd_info{};
	compute_dispatch_cmd_info.pipeline_instance = denoiser_resources.temporal_accumulation_instance.getHandle();
	compute_dispatch_cmd_info.size_x = size.x;
	compute_dispatch_cmd_info.size_y = size.y;
	context.cmdDispatch(compute_dispatch_cmd_info);
}

void Denoiser::blurIrradiance(Frame &frame, GBufferResources &gbuffer_resources) {

	denoiser_resources.vertical_blur_instance.setUniform("frame", &frame);
	denoiser_resources.horizontal_blur_instance.setUniform("frame", &frame);

	denoiser_resources.vertical_blur_instance.setImage("inputImage", gbuffer_resources.history_irradiance[frame.index % gbuffer_resources.history_irradiance.size()]);
	denoiser_resources.vertical_blur_instance.setImage("outputImage", gbuffer_resources.denoiser_irradiance_vertical_blur_texture.getHandle());

	denoiser_resources.horizontal_blur_instance.setImage("inputImage", gbuffer_resources.denoiser_irradiance_vertical_blur_texture.getHandle());
	denoiser_resources.horizontal_blur_instance.setImage("outputImage", gbuffer_resources.history_irradiance[frame.index % gbuffer_resources.history_irradiance.size()]);

	ComputeDispatchCmdInfo compute_dispatch_cmd_info{};
	compute_dispatch_cmd_info.pipeline_instance = denoiser_resources.vertical_blur_instance.getHandle();
	compute_dispatch_cmd_info.size_x = gbuffer_resources.denoiser_irradiance_vertical_blur_texture.getSize().x;
	compute_dispatch_cmd_info.size_y = gbuffer_resources.denoiser_irradiance_vertical_blur_texture.getSize().y;
	compute_dispatch_cmd_info.size_z = 1;
	context.cmdDispatch(compute_dispatch_cmd_info);
	compute_dispatch_cmd_info.pipeline_instance = denoiser_resources.horizontal_blur_instance.getHandle();
	context.cmdDispatch(compute_dispatch_cmd_info);
}

void Denoiser::setSceneUniforms(SceneResources &scene_resources) {
}

void Denoiser::setGBufferUniforms(GBufferResources &gbuffer_resources) {
	denoiser_resources.temporal_accumulation_instance.setImageArray("historyAlbedo", gbuffer_resources.history_albedo.data(), gbuffer_resources.history_albedo.size());
	denoiser_resources.temporal_accumulation_instance.setImageArray("historyNormalDepth", gbuffer_resources.history_normal_depth.data(), gbuffer_resources.history_normal_depth.size());
	denoiser_resources.temporal_accumulation_instance.setImageArray("historyMotion", gbuffer_resources.history_motion.data(), gbuffer_resources.history_motion.size());
	denoiser_resources.temporal_accumulation_instance.setImageArray("historyIrradiance", gbuffer_resources.history_irradiance.data(), gbuffer_resources.history_irradiance.size());
	denoiser_resources.temporal_accumulation_instance.setImageArray("historyPosition", gbuffer_resources.history_position.data(), gbuffer_resources.history_position.size());

	denoiser_resources.temporal_accumulation_instance.setImage("outputImage", gbuffer_resources.denoiser_temporal_accumulation_texture.getHandle());

	denoiser_resources.vertical_blur_instance.setImageArray("historyNormalDepth", gbuffer_resources.history_normal_depth.data(), gbuffer_resources.history_normal_depth.size());
	denoiser_resources.horizontal_blur_instance.setImageArray("historyNormalDepth", gbuffer_resources.history_normal_depth.data(), gbuffer_resources.history_normal_depth.size());
}

Denoiser::Denoiser(uint32_t history_count) : context(*Application::instance->getContext()) {
	//Accumulation
	std::string preamble = ("#define HISTORY_COUNT " + std::to_string(history_count) + "\n");
	denoiser_resources.temporal_accumulation_shader.loadGLSL("shaders/denoising/temporal_accumulation.comp",SHADER_STAGE_COMPUTE,preamble.c_str());

	ComputePipelineInfo accumulation_compute_pipeline_info{};
	accumulation_compute_pipeline_info.compute_shader = denoiser_resources.temporal_accumulation_shader.getHandle();
	denoiser_resources.temporal_accumulation_pipeline.alloc(accumulation_compute_pipeline_info);
	denoiser_resources.temporal_accumulation_pipeline.allocInstance(denoiser_resources.temporal_accumulation_instance);

	//BLUR
	denoiser_resources.horizontal_blur_shader.loadGLSL("shaders/denoising/horizontal_blur.comp",SHADER_STAGE_COMPUTE,preamble.c_str());
	denoiser_resources.vertical_blur_shader.loadGLSL("shaders/denoising/vertical_blur.comp",SHADER_STAGE_COMPUTE,preamble.c_str());

	ComputePipelineInfo blur_compute_pipeline_info{};
	blur_compute_pipeline_info.compute_shader = denoiser_resources.horizontal_blur_shader.getHandle();
	denoiser_resources.horizontal_blur_pipeline.alloc(blur_compute_pipeline_info);
	blur_compute_pipeline_info.compute_shader = denoiser_resources.vertical_blur_shader.getHandle();
	denoiser_resources.vertical_blur_pipeline.alloc(blur_compute_pipeline_info);

	denoiser_resources.horizontal_blur_pipeline.allocInstance(denoiser_resources.horizontal_blur_instance);
	denoiser_resources.vertical_blur_pipeline.allocInstance(denoiser_resources.vertical_blur_instance);
}

Denoiser::~Denoiser() {

}


