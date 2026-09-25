
#pragma once

#include "HBE.h"

struct InstancedMeshRenderer {
	COMPONENT_IDS(InstancedMeshRenderer)
	const void *instance_buffer_data;
	Handle instancing_entry_handle;
};
