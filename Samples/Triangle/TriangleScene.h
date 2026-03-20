#pragma once

#include "HBE.h"

using namespace HBE;

class TriangleScene : public Scene {
    Mesh triangle_mesh;
    Shader vertex_shader;
    Shader fragment_shader;
    RasterizationPipeline pipeline;
    PipelineInstance pipeline_instance;
    Entity triangle_entity;
    float vertices[9] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

public:
    TriangleScene() {
        //--------------------------Create Resources
        MeshInfo triangle_mesh_creation_info{};
        /*
         * Attribute infos describe the layout of a mesh’s buffers.
         *
         * For example, VERTEX_ATTRIBUTE_INFO_POSITION3D indicates that the mesh
         * contains a vec3 buffer bound to location 0.
         *
         * Multiple attribute infos can be combined to define different buffer layouts:
         * - Interleaved: a single buffer containing packed attributes (position/UVs/normals).
         * - Separate: individual buffers for each attribute (position, UVs, normals).
         */
        triangle_mesh_creation_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
        triangle_mesh_creation_info.attribute_info_count = 1;
        triangle_mesh_creation_info.flags = MESH_FLAG_NONE;
        triangle_mesh.alloc(triangle_mesh_creation_info);
        triangle_mesh.setBuffer(0, vertices, 3);

        fragment_shader.loadGLSL("shaders/defaults/Position.frag", SHADER_STAGE_FRAGMENT);
        vertex_shader.loadGLSL("shaders/defaults/Position.vert", SHADER_STAGE_VERTEX);

        RasterizationPipelineInfo pipeline_info{};
        //Specify that the pipeline will be compatible with one position attribute like the mesh.
        pipeline_info.attribute_info_count = 1;
        pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;;
        pipeline_info.vertex_shader = vertex_shader.getHandle();
        pipeline_info.fragment_shader = fragment_shader.getHandle();
        pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_NONE;

        pipeline.alloc(pipeline_info);
        //PipelineInstances are the container of the buffers that store the uniform/buffers/textures
        pipeline.allocInstance(pipeline_instance);

        vec4 color = {1, 0, 0, 1};
        pipeline_instance.setUniform("material", &color);

        //--------------------------Create Entities

        Entity camera_entity = createEntity3D();
        camera_entity.attach<Camera2D>();
        setCameraEntity(camera_entity);

        triangle_entity = createEntity3D();

        MeshRenderer *triangle_renderer = triangle_entity.attach<MeshRenderer>();
        triangle_renderer->mesh = triangle_mesh.getHandle();
        triangle_renderer->pipeline_instance = pipeline_instance.getHandle();
    }

    void createResources() {
    }

    void setupScene() {
    }
};
