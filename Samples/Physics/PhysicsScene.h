#include "PhysicsSystem.h"
#include "RigidBody.h"
#include "HBE/HBE.h"
#include "HBE/core/scene/systems/CameraControllerSystem.h"
#include "InstancedMeshRenderer.h"
#include "InstancedMeshRendererSystem.h"

using namespace HBE;

class PhysicsScene : public Scene {
	Mesh cube_mesh;
	Shader fragment_shader;
	Shader vertex_shader;
	RasterizationPipeline pipeline;
	PipelineInstance pipeline_instance;
	PhysicsSystem *physics_system;
	InstancedMeshRendererSystem *instanced_mesh_renderer_system;
	Handle instance_cube_entry;
	float last_spawn_time = 0.0f;
	float firerate = 0.05f;
public:
	void update(float deltaTime) {
		Scene::update(deltaTime);
		if(input.getKeyDown(KEY::KEY_C)) {
			Entity cam = getCameraEntity();
			if (cam.valid()) {
				if (cam.has<CameraController>()) {
					input.setCursorVisible(true);
					cam.detach<CameraController>();
				} else {
					input.setCursorVisible(false);
					cam.attach<CameraController>();
				}
			}
		}
		last_spawn_time += deltaTime;
		if (last_spawn_time>firerate && input.getKey(KEY::KEY_MOUSE_BUTTON_RIGHT)) {
			for (int i = 0; i < 10; ++i) {
				Entity e = createCubeEntity();
				e.get<Transform>()->setPosition(getCameraEntity().get<Transform>()->position() + getCameraEntity().get<Transform>()->worldBackward()*2.0f +
				Random::vec3Range(vec3(-0.5f, -0.5f, -0.5f), vec3(0.5f, 0.5f, 0.5f)));
				e.get<RigidBody>()->setVelocity(getCameraEntity().get<Transform>()->worldBackward()*Random::floatRange(10.0f, 30.0f));
			}

			last_spawn_time =0;
		}
	}
	PhysicsScene(): Scene({SCENE_INITIALIZE_SYSTEMS_FLAG_ALL}) {
		physics_system = addSystem<PhysicsSystem>(this);
		instanced_mesh_renderer_system = addSystem<InstancedMeshRendererSystem>(this);
		createResources();

		instance_cube_entry = instanced_mesh_renderer_system->addInstancedMesh(cube_mesh.getHandle(),
																			   pipeline_instance.getHandle(),
																			   sizeof(mat4),
																			   1);

		setupScene();

	}

	~PhysicsScene() {
	}

	void createResources() {
		fragment_shader.loadGLSL("shaders/InstancedBuffer.frag", SHADER_STAGE_FRAGMENT);
		vertex_shader.loadGLSL("shaders/InstancedBuffer.vert", SHADER_STAGE_VERTEX);

		RasterizationPipelineInfo pipeline_info{};
		pipeline_info.attribute_info_count = 1;
		pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
		pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_CULL_BACK;
		pipeline_info.fragment_shader = fragment_shader.getHandle();
		pipeline_info.vertex_shader = vertex_shader.getHandle();

		pipeline.alloc(pipeline_info);
		pipeline.allocInstance(pipeline_instance);

		MeshInfo mesh_info{};
		mesh_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
		mesh_info.attribute_info_count = 1;

		cube_mesh.alloc(mesh_info);
		Geometry::createCube(cube_mesh, 1, 1, 1, VERTEX_FLAG_NONE);
	}

	Entity createCubeEntity() {
		//create an empty entity then attach a transform
		Entity cube_entity = createEntity3D();

		//rendering
		InstancedMeshRenderer *cube_renderer = cube_entity.attach<InstancedMeshRenderer>();
		cube_renderer->instancing_entry_handle = instance_cube_entry;
		cube_renderer->instance_buffer_data = &cube_entity.get<Transform>()->local();

		//add physics
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
		ground_entity.get<RigidBody>()->setShapeBox(vec3(50, 0.5, 50));

		Entity wall_entity1 = createCubeEntity();
		wall_entity1.get<Transform>()->setLocalScale(vec3(1, 10, 100));
		wall_entity1.get<Transform>()->setPosition(vec3(50, -5, 0));
		wall_entity1.get<RigidBody>()->setDynamic(false);
		wall_entity1.get<RigidBody>()->setShapeBox(vec3(0.5, 5, 50));

		Entity wall_entity2 = createCubeEntity();
		wall_entity2.get<Transform>()->setLocalScale(vec3(1, 10, 100));
		wall_entity2.get<Transform>()->setPosition(vec3(-50, -5, 0));
		wall_entity2.get<RigidBody>()->setDynamic(false);
		wall_entity2.get<RigidBody>()->setShapeBox(vec3(0.5, 5, 50));

		Entity wall_entity3 = createCubeEntity();
		wall_entity3.get<Transform>()->setLocalScale(vec3(100, 10, 1));
		wall_entity3.get<Transform>()->setPosition(vec3(0, -5, 50));
		wall_entity3.get<RigidBody>()->setDynamic(false);
		wall_entity3.get<RigidBody>()->setShapeBox(vec3(50, 5, 0.5));

		Entity wall_entity4 = createCubeEntity();
		wall_entity4.get<Transform>()->setLocalScale(vec3(100, 10, 1));
		wall_entity4.get<Transform>()->setPosition(vec3(0, -5, -50));
		wall_entity4.get<RigidBody>()->setDynamic(false);
		wall_entity4.get<RigidBody>()->setShapeBox(vec3(50, 5, 0.5));
		//cubes
		for (uint i = 0; i < 50; i++) {
			Entity e = createCubeEntity();
			e.get<Transform>()->setPosition(vec3(Random::floatRange(-25, 25), Random::floatRange(0, 20), Random::floatRange(-25, 25)));
			e.get<Transform>()->translate(vec3(0, 10, 0));
		}

		Entity camera_entity = createEntity3D();
		camera_entity.attach<Camera>();
		camera_entity.attach<CameraController>();
		camera_entity.get<Transform>()->setPosition(vec3(0, 0, 50));
	}
};
