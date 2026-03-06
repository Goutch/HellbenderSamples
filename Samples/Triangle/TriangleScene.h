#pragma once

#include "HBE.h"

using namespace HBE;

class TriangleScene : public Scene
{
    Mesh triangle_mesh;
    Shader vertex_shader;
    Shader fragment_shader;
    RasterizationPipeline pipeline;
    PipelineInstance pipeline_instance;
    Entity triangle_entity;
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };
public:
    TriangleScene()
    {
        createResources();
        setupScene();
    }

    void createResources()
    {
        MeshInfo triangle_info{};
        triangle_info.attribute_info_count = 1;
        triangle_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
        triangle_mesh.alloc(triangle_info);
        triangle_mesh.setBuffer(0, vertices, 3);

        fragment_shader.loadGLSL("shaders/defaults/Position.frag", SHADER_STAGE_FRAGMENT);
        vertex_shader.loadGLSL("shaders/defaults/Position.vert", SHADER_STAGE_VERTEX);

        RasterizationPipelineInfo pipeline_info{};
        pipeline_info.attribute_info_count = 1;
        pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
        pipeline_info.vertex_shader = vertex_shader.getHandle();
        pipeline_info.fragment_shader = fragment_shader.getHandle();
        pipeline_info.flags = RASTERIZATION_PIPELINE_FLAG_NONE;

        pipeline.alloc(pipeline_info);
        pipeline.allocInstance(pipeline_instance);

        vec4 color = {1, 0, 0, 1};
        pipeline_instance.setUniform("material", &color);
    }

    void setupScene()
    {
        Entity camera_entity = createEntity3D();
        camera_entity.attach<Camera2D>();
        setCameraEntity(camera_entity);

        triangle_entity = createEntity3D();
        MeshRenderer* triangle_renderer = triangle_entity.attach<MeshRenderer>();
        triangle_renderer->mesh = triangle_mesh.getHandle();
        triangle_renderer->pipeline_instance = pipeline_instance.getHandle();
    }
};
