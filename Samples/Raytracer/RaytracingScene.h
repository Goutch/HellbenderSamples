#pragma once

#include "HBE.h"
#include "vector"
#include "array"
#include "DataStructures.h"
#include "RaytracingModelParser.h"
#include "Raytracer.h"
//#include "NRD.h"

using namespace HBE;


#define HISTORY_COUNT 4

class RaytracingScene : public Scene {
	enum MATERIALS {
		MATERIAL_WHITE = 0,
		MATERIAL_RED = 1,
		MATERIAL_GREEN = 2,
		MATERIAL_BLUE = 3,
		MATERIAL_YELLOW = 4,
		MATERIAL_PURPLE = 5,
		MATERIAL_METALIC = 6,
		MATERIAL_MIRROR = 7,
		MATERIAL_LIGHT = 9,
	};
	enum SHADER_GROUP_TYPE {
		SHADER_GROUP_TYPE_BOX = 0,
		SHADER_GROUP_TYPE_SPHERE = 1,
		SHADER_GROUP_TYPE_MESH = 2,
	};
	enum RENDER_MODE {
		NORMAL = 0,
		DEPTH = 1,
		ALBEDO = 2,
		IRRADIANCE = 3,
		RAW = 4,
		ACCUMULATED = 5,
		DENOISED = 6,
	};
private:
	GBufferResources gbuffer_resources;
	SceneResources scene_resources;
	DenoisingResources denoising_resources;
	Raytracer *raytracer;
	Frame frame{};

	//assets
	RaytracingModelParser *model_parser;
	Model *sponza_model;


	bool paused = false;
	float time = 0;
	RENDER_MODE render_mode = ALBEDO;
public:

	void createGBuffer(uint32_t width, uint32_t height);

	void createDenoisingResources();

	void onResolutionChange(RenderTarget *rt);

	void createRaytracingResources();

	void loadAssets();

	void createScene();

	void render() override;

	Image *getMainCameraTexture() override;

	void update(float delta) override;

	~RaytracingScene() override;

	void createSphereField(int n);

	void createMaterialDisplay();

	void createCornelBox();

	RaytracingScene();
};


