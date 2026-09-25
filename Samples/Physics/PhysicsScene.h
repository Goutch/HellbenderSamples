#include "PhysicsSystem.h"
#include "RigidBody.h"
#include "HBE/HBE.h"
#include "HBE/core/scene/systems/CameraControllerSystem.h"

using namespace HBE;

class PhysicsScene : public Scene {
	Mesh cube_mesh;
	Shader fragment_shader;
	Shader vertex_shader;
	RasterizationPipeline pipeline;
	PipelineInstance red_pipeline_instance;
	PipelineInstance gray_pipeline_instance;
	PhysicsSystem *physics_system;

public:
	void update(float deltaTime) {
		Scene::update(deltaTime);
		if (input.getKeyDown(KEY::KEY_MOUSE_BUTTON_RIGHT)) {
			Entity e = createCubeEntity();
			e.get<Transform>()->setPosition(getCameraEntity().get<Transform>()->position());
			e.get<RigidBody>()->setVelocity(getCameraEntity().get<Transform>()->worldBackward()*10.0f);
		}
	}
	PhysicsScene() {
		physics_system = new PhysicsSystem(this);
		addSystem(physics_system);
		addSystem(new CameraControllerSystem(this));
		createResources();
		setupScene();
	}

	~PhysicsScene() {
	}

	void createResources() {
		fragment_shader.loadGLSL("shaders/defaults/Position.frag", SHADER_STAGE_FRAGMENT);
		vertex_shader.loadGLSL("shaders/defaults/Position.vert", SHADER_STAGE_VERTEX);

		RasterizationPipelineInfo pipeline_info{};
		pipeline_info.attribute_info_count = 1;
		pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
		pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_CULL_BACK;
		pipeline_info.fragment_shader = fragment_shader.getHandle();
		pipeline_info.vertex_shader = vertex_shader.getHandle();

		pipeline.alloc(pipeline_info);
		pipeline.allocInstance(red_pipeline_instance);
		pipeline.allocInstance(gray_pipeline_instance);

		MeshInfo mesh_info{};
		mesh_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
		mesh_info.attribute_info_count = 1;

		cube_mesh.alloc(mesh_info);
		Geometry::createCube(cube_mesh, 1, 1, 1, VERTEX_FLAG_NONE);
	}

	Entity createCubeEntity() {
		Entity cube_entity = createEntity3D();
		MeshRenderer *cube_renderer = cube_entity.attach<MeshRenderer>();
		cube_renderer->mesh = cube_mesh.getHandle();
		cube_renderer->pipeline_instance = red_pipeline_instance.getHandle();

		cube_entity.get<Transform>()->translate(vec3(0, 0, -5));
		RigidBody *rigid_body = cube_entity.attach<RigidBody>();
		rigid_body->setDynamic(true);

		return cube_entity;
	}

	void setupScene() {

		//ground
		Entity ground_entity = createCubeEntity();
		ground_entity.get<Transform>()->setLocalScale(vec3(100, 1, 100));
		ground_entity.get<Transform>()->setPosition(vec3(0, -5, 0));
		ground_entity.get<RigidBody>()->setDynamic(false);
		ground_entity.get<RigidBody>()->setShapeBox(vec3(100, 1, 100));
		ground_entity.get<MeshRenderer>()->pipeline_instance = gray_pipeline_instance.getHandle();
		//cubes
		for (uint i = 0; i < 1000; i++) {
			Entity e = createCubeEntity();
			e.get<Transform>()->setPosition(vec3(Random::floatRange(-50, 50), Random::floatRange(-5, 5), Random::floatRange(-5, 5)));
			e.get<Transform>()->translate(vec3(0, 10, 0));
		}

		Entity camera_entity = createEntity3D();
		camera_entity.attach<Camera>();
		camera_entity.attach<CameraController>();

		vec4 c = vec4(1, 0, 0, 1);
		red_pipeline_instance.setUniform("material", &c);
		c = vec4(0.2, 0.2, 0.2, 1);
		gray_pipeline_instance.setUniform("material", &c);
	}
};
