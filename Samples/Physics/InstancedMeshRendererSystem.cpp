
#include "InstancedMeshRendererSystem.h"
#include "dependencies/utils-collection/Profiler.h"
#include <core/scene/Scene.h>
#include <core/graphics/RenderGraph.h>
#include "core/scene/components/Transform.h"
#include "core/scene/components/Node.h"
#include "core/scene/components/MeshRenderer.h"
#include "InstancedMeshRenderer.h"

InstancedMeshRendererSystem::InstancedMeshRendererSystem(Scene *scene) : System(scene) {
	scene->onDraw.subscribe(on_draw_subscription_id, this, &InstancedMeshRendererSystem::onDraw);
}

void InstancedMeshRendererSystem::onDraw(RenderGraph *render_graph) {
	HB_PROFILE_BEGIN("InstancedMeshRendererDraw");
	DrawCmdInfo draw_cmd{};
	auto group = scene->group<Transform, InstancedMeshRenderer>();
	uint cached_push_constants_count = 0;
	PushConstantInfo *push_constant_infos = nullptr;

	//count instances;
	for (auto [handle, transform, mesh_renderer]: group) {
		InstancingEntry &entry = instancing_entries[instancing_entry_handle_provider.index(mesh_renderer.instancing_entry_handle)];
		entry.instance_count++;
	}
	//realloc out of range buffers
	for (int i = 0; i < instancing_entries.size(); ++i) {
		InstancingEntry &entry = instancing_entries[i];
		if (entry.instance_count > entry.instance_buffer_capacity) {
			if (entry.data != nullptr)
				delete[] entry.data;

			entry.data = new uint8_t[entry.instance_count * entry.instance_buffer_element_size];
			entry.instance_buffer_capacity = entry.instance_count;
		}
	}
	for (auto [handle,transform, mesh_renderer]: group) {
		InstancingEntry &entry = instancing_entries[instancing_entry_handle_provider.index(mesh_renderer.instancing_entry_handle)];
		memcpy(entry.data + (entry.current_index * entry.instance_buffer_element_size), mesh_renderer.instance_buffer_data, entry.instance_buffer_element_size);
		entry.current_index++;
	}
	//rebuild storage buffers with out of range counts
	for (int i = 0; i < instancing_entries.size(); ++i) {
		InstancingEntry &entry = instancing_entries[i];
		if (!entry.instance_buffer.allocated() || entry.instance_count > entry.instance_buffer.getCount()) {
			BufferInfo buffer_info{};
			buffer_info.count = entry.instance_count;
			buffer_info.stride = entry.instance_buffer_element_size;
			buffer_info.usage = BUFFER_USAGE_FLAG_STORAGE_BUFFER;
			buffer_info.preferred_memory_type_flags = MEMORY_TYPE_FLAG_MAPPABLE;
			buffer_info.optional_data = entry.data;
			if(!entry.instance_buffer.allocated())
				entry.instance_buffer.alloc(buffer_info);
			else
				entry.instance_buffer.resize(buffer_info);

			context.setPipelineInstanceStorageBuffer(entry.pipeline_instance, entry.instance_buffer_binding, entry.instance_buffer.getHandle(), 0);
		}
		else
		{
			entry.instance_buffer.update(entry.data);
		}
	}

	for (int i = 0; i < instancing_entries.size(); ++i) {
		InstancingEntry &entry = instancing_entries[i];
		if (entry.instance_count == 0)
			continue;

		draw_cmd.push_constants_count = 0;
		draw_cmd.push_constants = push_constant_infos;
		draw_cmd.mesh = entry.mesh;
		draw_cmd.pipeline_instance_handle = entry.pipeline_instance;
		context.getPipelineFromInstance(entry.pipeline_instance, draw_cmd.rasterization_pipeline_handle);
		draw_cmd.layer = 0;

		draw_cmd.flags = DRAW_CMD_FLAG_NONE;
		draw_cmd.order_in_layer = 0;
		draw_cmd.instance_count = entry.instance_count;
		render_graph->add(draw_cmd);

		//reset it
		instancing_entries[i].instance_count = 0;
		instancing_entries[i].current_index = 0;
	}
	HB_PROFILE_END("InstancedMeshRendererDraw");
}

InstancedMeshRendererSystem::~InstancedMeshRendererSystem() {
	scene->onDraw.unsubscribe(on_draw_subscription_id);
}

Handle InstancedMeshRendererSystem::addInstancedMesh(MeshHandle mesh, PipelineInstanceHandle pipeline_instance, uint32_t instance_buffer_element_size, uint32_t instance_buffer_binding) {
	Handle handle = instancing_entry_handle_provider.create();
	instancing_entries.resize(instancing_entry_handle_provider.size());
	InstancingEntry &entry = instancing_entries[instancing_entry_handle_provider.index(handle)];
	entry.mesh = mesh;
	entry.pipeline_instance = pipeline_instance;
	entry.instance_buffer_element_size = instance_buffer_element_size;
	entry.instance_buffer_binding = instance_buffer_binding;
	return handle;
}

void InstancedMeshRendererSystem::removeInstancedMesh(Handle instancing_entry_handle) {
	instancing_entries[instancing_entry_handle_provider.index(instancing_entry_handle)].instance_buffer.release();
	instancing_entry_handle_provider.release(instancing_entry_handle);
}


