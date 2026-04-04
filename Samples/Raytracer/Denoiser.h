
#pragma once
#include "DataStructures.h"

class Denoiser {

private :
	Context &context;
	DenoisingResources denoiser_resources;
public:
	Denoiser(uint32_t history_count);
	~Denoiser();
	void setGBufferUniforms(GBufferResources &gbuffer_resources);

	void setSceneUniforms(SceneResources &scene_resources);

	void accumulate(Frame &frame, GBufferResources &gbuffer_resources);
	void blurIrradiance(Frame &frame, GBufferResources &gbuffer_resources);
};
