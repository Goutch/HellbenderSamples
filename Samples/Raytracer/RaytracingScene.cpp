
#include "RaytracingScene.h"

void RaytracingScene::render() {
	Scene::render();
	Entity camera_entity = getCameraEntity();
	if (!paused)
		frame.time = time;

	mat4 camera_projection = camera_entity.get<Camera>()->projection;
	mat4 camera_view = camera_entity.get<Transform>()->world();
	gbuffer_resources.history_camera[frame.index % HISTORY_COUNT] = {camera_view, camera_projection};

	///--------------------------------RAYTRACING--------------------------------///
	raytracer.traceRays(frame, gbuffer_resources, &scene_resources.root_acceleration_structure);

	//--------------------------------DENOISING--------------------------------/
	if (render_mode >= BLURRED_IRRADIANCE) {
		denoiser.blurIrradiance(frame, gbuffer_resources);
	}
	if (render_mode >= ACCUMULATED) {
		denoiser.accumulate(frame, gbuffer_resources);
	}
	frame.index++;
}

void RaytracingScene::loadAssets() {
	scene_resources.materials = {{
			                             {vec4(0.95, 0.95, 0.95, 0.0), vec4(0.0, 0.0, 0.0, 0.0), -1, -1, 1.0},//white
			                             {vec4(0.95, 0, 0, 0.0), vec4(0.0, 0.0, 0.0, 0.0), -1, -1, 1.0},//red
			                             {vec4(0, 0.95, 0, 0.0), vec4(0.0, 0.0, 0.0, 0.0), -1, -1, 1.0},//green
			                             {vec4(0, 0, 0.95, 0.0), vec4(0.0, 0.0, 0.0, 0.0), -1, -1, 1.0},//blue
			                             {vec4(0.9, 0.9, 0.1, 0.0), vec4(0.0, 0.0, 0.0, 0.0), -1, -1, 1.0},//yellow
			                             {vec4(0.9, 0.1, 0.9, 0.0), vec4(0.0, 0.0, 0.0, 0.0), -1, -1, 1.0},//purple
			                             {vec4(0.8, 0.8, 0.8, 0.0), vec4(0.0, 0.0, 0.0, 0.0), -1, -1, 0.4},//metalic
			                             {vec4(0.8, 0.8, 0.8, 0.0), vec4(0.0, 0.0, 0.0, 0.0), -1, -1, 0.0},//mirror
			                             {vec4(0.5, 0.5, 0.5, 0.0), vec4(10.0, 10.0, 10.0, 10.0), -1, -1, 1.0}}//light
	};

	RaytracingModelParserInfo model_parser_info{};
	model_parser_info.mesh_shader_group_index = SHADER_GROUP_TYPE_MESH;
	model_parser_info.acceleration_structures = &scene_resources.mesh_acceleration_structures;
	model_parser_info.materials = &scene_resources.materials;
	model_parser_info.textures = &scene_resources.textures;
	model_parser_info.meshes = &scene_resources.meshes;

	model_parser = new RaytracingModelParser(model_parser_info);

	ModelInfo model_info{};
	model_info.flags = MODEL_FLAG_USED_IN_RAYTRACING;
	model_info.parser = model_parser;
	model_info.path = "/models/sponza/Sponza.gltf";

	sponza_model.load(model_info);

	delete model_parser;
}

void RaytracingScene::createScene() {

	mat4 transform_aabb_floor(1.0f);
	transform_aabb_floor = glm::translate(transform_aabb_floor, vec3(0, -1, 0));
	transform_aabb_floor = glm::scale(transform_aabb_floor, vec3(1000, 1, 1000));

	//todo: create a single bottom level acceleration structure from mesh inside the model
	std::vector<AccelerationStructureInstance> sponza_acceleration_structure_instances = sponza_model.getAccelerationStructureInstances();
	scene_resources.acceleration_structure_instances.insert(scene_resources.acceleration_structure_instances.end(), sponza_acceleration_structure_instances.begin(),
	                                                        sponza_acceleration_structure_instances.end());

	scene_resources.acceleration_structure_instances.push_back(AccelerationStructureInstance{0,
	                                                                                         SHADER_GROUP_TYPE_BOX,
	                                                                                         transform_aabb_floor,
	                                                                                         ACCELERATION_STRUCTURE_TYPE_AABB,
	                                                                                         0});

	Entity camera_entity = createEntity3D();
	camera_entity.attach<Camera>();
	//camera_entity.get<Camera>().setRenderTarget(Graphics::getDefaultRenderTarget());
	camera_entity.get<Camera>()->active = false;
	camera_entity.get<Transform>()->translate(vec3(0, 1, 0));
	camera_entity.get<Transform>()->rotate(vec3(0, 2.1416, 0));
	CameraController *controller = camera_entity.attach<CameraController>();
	Camera *camera = camera_entity.get<Camera>();
	setCameraEntity(camera_entity);
	gbuffer_resources.history_camera.resize(HISTORY_COUNT, {camera_entity.get<Transform>()->world(), camera->projection});
}

RaytracingScene::RaytracingScene() {

	createGBuffer(window.getSize().x, window.getSize().y);

	loadAssets();
	createScene();

	AABBAccelerationStructureInfo aabb__acceleration_structure_info{};
	aabb__acceleration_structure_info.max = vec3(0.5, 0.5, 0.5);
	aabb__acceleration_structure_info.min = vec3(-0.5, -0.5, -0.5);
	scene_resources.aabb_acceleration_structure.alloc(aabb__acceleration_structure_info);

	RootAccelerationStructureInfo root_acceleration_structure_info{};
	root_acceleration_structure_info.aabb_acceleration_structures = &scene_resources.aabb_acceleration_structure.getHandleRef();
	root_acceleration_structure_info.aabb_acceleration_structure_count = 1;
	root_acceleration_structure_info.mesh_acceleration_structures = scene_resources.mesh_acceleration_structures.data();
	root_acceleration_structure_info.mesh_acceleration_structure_count = scene_resources.mesh_acceleration_structures.size();
	root_acceleration_structure_info.instances = scene_resources.acceleration_structure_instances.data();
	root_acceleration_structure_info.instance_count = scene_resources.acceleration_structure_instances.size();

	scene_resources.root_acceleration_structure.alloc(root_acceleration_structure_info);

	BufferInfo material_storage_buffer_info{};
	material_storage_buffer_info.stride = sizeof(MaterialData);
	material_storage_buffer_info.count = scene_resources.materials.size();
	material_storage_buffer_info.optional_data = scene_resources.materials.data();
	material_storage_buffer_info.flags = BUFFER_FLAG_NONE;
	scene_resources.material_buffer.alloc(material_storage_buffer_info);

	std::vector<InstanceInfo> instance_infos;
	for (uint32_t i = 0; i < scene_resources.acceleration_structure_instances.size(); ++i) {
		instance_infos.push_back({scene_resources.acceleration_structure_instances[i].custom_index, i, 16});
	}
	BufferInfo instances_storage_buffer_info{};
	instances_storage_buffer_info.stride = sizeof(InstanceInfo);
	instances_storage_buffer_info.count = instance_infos.size();
	instances_storage_buffer_info.optional_data = instance_infos.data();
	instances_storage_buffer_info.flags = BUFFER_FLAG_NONE;
	scene_resources.instance_buffer.alloc(instances_storage_buffer_info);


	for (uint32_t i = 0; i < scene_resources.meshes.size(); i++) {
		Handle handle;
		context.getMeshAttributeBuffer(scene_resources.meshes[i],2,handle);
		scene_resources.normals.push_back(handle);
		context.getMeshAttributeBuffer(scene_resources.meshes[i],1,handle);
		scene_resources.uvs.push_back(handle);
		context.getMeshIndicesBuffer(scene_resources.meshes[i],handle);
		scene_resources.indices.push_back(handle);

	}

	raytracer.setGBufferUniforms(gbuffer_resources);
	raytracer.setSceneUniforms(scene_resources);

	denoiser.setGBufferUniforms(gbuffer_resources);
	denoiser.setSceneUniforms(scene_resources);

	window.onSizeChange.subscribe(on_resolution_change_subscription_id, this, &RaytracingScene::onResolutionChange);
}

void RaytracingScene::destroyGBuffer() {
	for (ImageHandle albedo: gbuffer_resources.history_albedo) {
		context.releaseImage(albedo);
	}
	for (ImageHandle normal_depth: gbuffer_resources.history_normal_depth) {
		context.releaseImage(normal_depth);
	}
	for (ImageHandle motion: gbuffer_resources.history_motion) {
		context.releaseImage(motion);
	}
	for (ImageHandle irradiance: gbuffer_resources.history_irradiance) {
		context.releaseImage(irradiance);
	}
	for (ImageHandle position: gbuffer_resources.history_position) {
		context.releaseImage(position);
	}

	gbuffer_resources.history_albedo.clear();
	gbuffer_resources.history_normal_depth.clear();
	gbuffer_resources.history_motion.clear();
	gbuffer_resources.history_irradiance.clear();
	gbuffer_resources.history_position.clear();
}

void RaytracingScene::createGBuffer(uint32_t width, uint32_t height) {
	destroyGBuffer();

	ImageInfo info{};
	info.width = width;
	info.height = height;
	info.format = IMAGE_FORMAT_RGBA32F;
	info.flags = IMAGE_FLAG_SHADER_WRITE;
	info.sampler_info.address_mode = IMAGE_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	info.sampler_info.filter = IMAGE_SAMPLER_FILTER_TYPE_NEAREST;
	info.sampler_info.flags = IMAGE_SAMPLER_FLAG_NONE;

	for (uint32_t i = 0; i < HISTORY_COUNT; i++) {
		gbuffer_resources.history_albedo.push_back(Image::create(info));
		gbuffer_resources.history_normal_depth.push_back(Image::create(info));
		gbuffer_resources.history_irradiance.push_back(Image::create(info));
		gbuffer_resources.history_motion.push_back(Image::create(info));
		gbuffer_resources.history_position.push_back(Image::create(info));
	}

	gbuffer_resources.denoiser_temporal_accumulation_texture.alloc(info);
	gbuffer_resources.denoiser_irradiance_vertical_blur_texture.alloc(info);
}

void RaytracingScene::onResolutionChange(Window *window) {
	createGBuffer(window->getSize().x, window->getSize().y);
}

ImageHandle RaytracingScene::getMainCameraTexture() {
	switch (render_mode) {
		case BLURRED_IRRADIANCE:
			return gbuffer_resources.history_irradiance[(frame.index - 1) % HISTORY_COUNT];
			break;
		case ACCUMULATED:
			return gbuffer_resources.denoiser_temporal_accumulation_texture.getHandle();
			break;
		case IRRADIANCE:
			return gbuffer_resources.history_irradiance[(frame.index - 1) % HISTORY_COUNT];
			break;
		case ALBEDO:
			return gbuffer_resources.history_albedo[(frame.index - 1) % HISTORY_COUNT];
			break;
		case NORMAL:
			return gbuffer_resources.history_normal_depth[(frame.index - 1) % HISTORY_COUNT];
			break;
		default:
			return gbuffer_resources.history_albedo[(frame.index - 1) % HISTORY_COUNT];
	}

}

void RaytracingScene::update(float delta) {
	Scene::update(delta);

	if (input.getKeyDown(KEY_NUMBER_1)) {
		render_mode = ACCUMULATED;
	}
	if (input.getKeyDown(KEY_NUMBER_2)) {
		render_mode = BLURRED_IRRADIANCE;
	}
	if (input.getKeyDown(KEY_NUMBER_3)) {
		render_mode = IRRADIANCE;
	}
	if (input.getKeyDown(KEY_NUMBER_4)) {
		render_mode = ALBEDO;
	}
	if (input.getKeyDown(KEY_NUMBER_5)) {
		render_mode = NORMAL;
	}
	if (input.getKey(KEY_NUMBER_7)) {
		frame.gamma -= delta;
		Log::message("Gamma:" + std::to_string(frame.gamma));
	}
	if (input.getKey(KEY_NUMBER_8)) {
		frame.gamma += delta;
		Log::message("Gamma:" + std::to_string(frame.gamma));
	}
	if (input.getKey(KEY_NUMBER_9)) {
		frame.exposure -= delta;
		Log::message("Exposure:" + std::to_string(frame.exposure));
	}
	if (input.getKey(KEY_NUMBER_0)) {
		frame.exposure += delta;
		Log::message("Exposure:" + std::to_string(frame.exposure));
	}
	if (input.getKeyDown(KEY_N)) {
		frame.use_blue_noise = uint32_t(!bool(frame.use_blue_noise));
	}

	if (input.getKeyDown(KEY_P)) {
		paused = !paused;
	}
	if (input.getKey(KEY_MINUS)) {
		frame.scattering_multiplier -= 5.0f * delta;
		Log::message("Scattering multiplier:" + std::to_string(frame.scattering_multiplier));
	}
	if (input.getKey(KEY_EQUAL)) {
		frame.scattering_multiplier += 5.0f * delta;

		Log::message("Scattering multiplier:" + std::to_string(frame.scattering_multiplier));
	}
	if (input.getKey(KEY_LEFT_BRACKET)) {
		frame.density_falloff -= 5.0f * delta;
		Log::message("Density falloff:" + std::to_string(frame.density_falloff));
	}
	if (input.getKey(KEY_RIGHT_BRACKET)) {
		frame.density_falloff += 5.0f * delta;
		Log::message("Density falloff:" + std::to_string(frame.density_falloff));
	}
	if (input.getKeyDown(KEY_UP)) {
		frame.sample_count++;
		Log::message("Sample count:" + std::to_string(frame.sample_count));
	}
	if (input.getKeyDown(KEY_DOWN)) {
		if (frame.sample_count > 0) {
			frame.sample_count--;
		}
		Log::message("Sample count:" + std::to_string(frame.sample_count));
	}
	if (input.getKeyDown(KEY_RIGHT)) {
		frame.max_bounces++;
		Log::message("Max bounces:" + std::to_string(frame.max_bounces));
	}
	if (input.getKeyDown(KEY_LEFT)) {
		if (frame.max_bounces > 0) {
			frame.max_bounces--;
		}
		Log::message("Max bounces:" + std::to_string(frame.max_bounces));
	}

	if (!paused)
		time += delta * 0.05;
}

RaytracingScene::~RaytracingScene() {
	window.onSizeChange.unsubscribe(on_resolution_change_subscription_id);

	destroyGBuffer();
	delete model_parser;
}

void RaytracingScene::createSphereField(int n) {
	for (uint32_t i = 0; i < n; ++i) {
		float radius = Random::floatRange(0.5, 2.5);
		Transform t{};
		t.translate(vec3(Random::floatRange(-50, 50), radius, Random::floatRange(-50, 50)));
		t.setLocalScale(vec3(radius * 2.0f));
		scene_resources.acceleration_structure_instances.push_back(
				AccelerationStructureInstance{0, SHADER_GROUP_TYPE_SPHERE, t.local(), ACCELERATION_STRUCTURE_TYPE_AABB, Random::uintRange(0, 8)});
	}
}

void RaytracingScene::createMaterialDisplay() {
	for (uint32_t i = 0; i < 10; ++i) {
		mat4 t(1.0f);
		t = glm::translate(t, vec3(2 * (i + 1), 0, 0));
		scene_resources.acceleration_structure_instances.push_back(
				AccelerationStructureInstance{0, SHADER_GROUP_TYPE_BOX, t, ACCELERATION_STRUCTURE_TYPE_AABB, i});
	}
	for (uint32_t i = 0; i < 7; ++i) {
		mat4 t(1.0f);
		t = glm::translate(t, vec3((2 * i) + 1, 0, 0));
		scene_resources.acceleration_structure_instances.push_back(
				AccelerationStructureInstance{0, SHADER_GROUP_TYPE_SPHERE, t, ACCELERATION_STRUCTURE_TYPE_AABB, i % 7});
	}
}





