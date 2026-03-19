#pragma once

#include "HBE.h"

using namespace HBE;

namespace Pong {
    class FPSCounter {
        Shader text_vertex_shader;
        Shader text_fragment_shader;

        RasterizationPipeline text_pipeline;

        PipelineInstance text_pipeline_instance;
        Entity text_entity;

        Mesh quad;

        Mesh text_mesh;
        Font font;

        std::string text;

        vec2 size;

        RasterizationTarget *render_target;
        Scene *scene;

        vec4 red = vec4(251.0f / 255.0f, 73.0f / 255.0f, 52.0f / 255.0f, 1);
        vec4 yellow = vec4(250.0f / 255.0f, 189.0f / 255.0f, 47.0f / 255.0f, 1);
        vec4 green = vec4(184.0f / 255.0f, 250.0f / 255.0f, 38.0f / 255.0f, 1);

        float time_since_last_fps_update = 0.0f;

        event_subscription_id update_subscription_id;
        event_subscription_id render_target_size_changed_subscription_id;

    public:
        void update(float delta) {
            time_since_last_fps_update += delta;
            if (time_since_last_fps_update < 1.0f) {
                return;
            }
            time_since_last_fps_update = 0.0f;
            int fps = (int) std::floor(1.0f / delta);

            text = std::to_string(fps) + "fps\n" +
                   std::to_string(delta * 1000.0f) + "ms";


            vec4 text_color = vec4(1, 1, 1, 1);

            if (fps < 60)
                text_color = red;
            else if (fps < 120) {
                text_color = yellow;
            } else {
                text_color = green;
            }
            text_pipeline_instance.setUniform("material", &text_color);

            Geometry::updateText(*Application::instance->getContext(),
                                 text_mesh.getHandleRef(),
                                 text,
                                 font,
                                 1.0,
                                 1.0,
                                 TEXT_ALIGNMENT_LEFT,
                                 PIVOT_TOP_LEFT,
                                 size);
        }

        void onResolutionChange(RasterizationTarget *render_target) {
            text_entity.get<Transform>()->setLocalScale(vec3(20));

            text_entity.get<Transform>()->setPosition(vec3(
                0,
                render_target->getResolution().y,
                0.0f));
        }


        FPSCounter(Scene &scene, RasterizationTarget *render_target) {
            this->scene = &scene;
            scene.onUpdate.subscribe(update_subscription_id, this, &FPSCounter::update);
            render_target->onResolutionChange.subscribe(render_target_size_changed_subscription_id, this, &FPSCounter::onResolutionChange);
            this->render_target = render_target;
            createResources();
            setupScene();
            onResolutionChange(render_target);
        }

        ~FPSCounter() {
            text_entity.destroy();
            scene->onUpdate.unsubscribe(update_subscription_id);
            render_target->onResolutionChange.unsubscribe(render_target_size_changed_subscription_id);
        }


        void setupScene() {
            text_entity = scene->createEntity3D();
            MeshRenderer *text_renderer = text_entity.attach<MeshRenderer>();

            text_renderer->pipeline_instance = text_pipeline_instance.getHandle();
            text_renderer->mesh = text_mesh.getHandle();
            text_renderer->ordered = true;

            text_entity.get<Transform>()->translate(vec3(1.0f, 1.0f, 0.0f));
        }

        void createResources() {
            text_vertex_shader.loadGLSL("shaders/defaults/TextMSDF.vert", SHADER_STAGE_VERTEX);
            text_fragment_shader.loadGLSL("shaders/defaults/TextMSDF.frag", SHADER_STAGE_FRAGMENT);

            RasterizationPipelineInfo pipeline_info{};
            pipeline_info.attribute_info_count = 1;

            pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
            pipeline_info.vertex_shader = text_vertex_shader.getHandle();
            pipeline_info.fragment_shader = text_fragment_shader.getHandle();
            pipeline_info.flags = 0;
            pipeline_info.rasterization_target = render_target->getHandle();
            text_pipeline.alloc(pipeline_info);
            text_pipeline.allocInstance(text_pipeline_instance);

            MeshInfo mesh_info{};
            mesh_info.attribute_info_count = 1;
            mesh_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
            quad.alloc(mesh_info);
            Geometry::createQuad(quad, 1, 1, VERTEX_FLAG_UV, PIVOT_TOP_LEFT);

            std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{};':\",./<>?\\|`~";
            FontInfo font_info{};
            font_info.path = "fonts/OpenSans-Regular.ttf";
            font_info.characters = characters.data();
            font_info.characters_count = characters.size();
            font_info.glyph_resolution = 64;

            font.load(font_info);


            text_pipeline_instance.setImage("mtsdf", font.getTextureAtlas()->getHandle());
            text = std::string("fps\n") + "ms";
            Geometry::updateText(*Application::instance->getContext(),
                                 text_mesh.getHandleRef(),
                                 text,
                                 font,
                                 1.0,
                                 1.0,
                                 TEXT_ALIGNMENT_LEFT,
                                 PIVOT_CENTER,
                                 size);
        }

        RasterizationPipeline &getPipeline() {
            return text_pipeline;
        }

        Font &getFont() {
            return font;
        }

        Shader &getVertexShader() {
            return text_vertex_shader;
        }

        Shader &getFragmentShader() {
            return text_fragment_shader;
        }
    };
}
