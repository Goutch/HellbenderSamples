
#pragma once

#include "HBE.h"

class InstancedMeshRendererSystem : public System {

	struct InstancingEntry {
		MeshHandle mesh;
		PipelineInstanceHandle pipeline_instance;
		StorageBuffer instance_buffer;
		uint32_t instance_buffer_element_size;
		uint32_t instance_count=0;
		uint32_t instance_buffer_binding;
		uint32_t current_index=0;
		uint32_t instance_buffer_capacity=0;
		uint8_t * data;
	};
	event_subscription_id on_draw_subscription_id = HBE_NULL_HANDLE;
	HandleProvider instancing_entry_handle_provider;
	std::vector<InstancingEntry> instancing_entries;
public:
	SYSTEM_IDS(InstancedMeshRendererSystem)
	InstancedMeshRendererSystem(Scene *scene);

	~InstancedMeshRendererSystem();

	void onDraw(RenderGraph *render_graph);

	Handle addInstancedMesh(MeshHandle mesh, PipelineInstanceHandle pipeline_instance, uint32_t instance_buffer_element_size,uint32_t instance_buffer_binding);
	void removeInstancedMesh(Handle instancing_entry_handle);
};
