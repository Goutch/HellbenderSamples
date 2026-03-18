#include "HBE/HBE.h"

using namespace HBE;

class CubeScene : public Scene {
	Mesh cube_mesh;
	Shader fragment_shader;
	Shader vertex_shader;
	RasterizationPipeline pipeline;
	PipelineInstance pipeline_instance;
public:

	CubeScene() {
		createResources();
		setupScene();
	}

	void createResources() {
		fragment_shader.loadGLSL("shaders/defaults/Position.frag",SHADER_STAGE_FRAGMENT);
		vertex_shader.loadGLSL("shaders/defaults/Position.vert",SHADER_STAGE_VERTEX);

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

	void createCubeMesh() {
		Entity cube_entity = createEntity3D();
		MeshRenderer *cube_renderer = cube_entity.attach<MeshRenderer>();
		cube_renderer->mesh = cube_mesh.getHandle();
		cube_renderer->pipeline_instance = pipeline_instance.getHandle();

		cube_entity.get<Transform>()->translate(vec3(0, 0, -5));
	}

	void setupScene() {
		createCubeMesh();

		Entity camera_entity = createEntity3D();
		camera_entity.attach<Camera>();

		vec4 c = vec4(1, 0, 0, 1);
		pipeline_instance.setUniform("material", &c);
	}

};