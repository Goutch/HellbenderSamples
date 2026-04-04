
#pragma once

#include "HBE.h"
#include "DataStructures.h"

struct RaytracingModelParserInfo {
	int mesh_shader_group_index;
	std::vector<ImageHandle> *textures;
	std::vector<MaterialData> *materials;
	std::vector<MeshHandle> *meshes;
	std::vector<MeshAccelerationStructureHandle> *acceleration_structures;
};

class RaytracingModelParser : public HBE::DefaultModelParser {
	RaytracingModelParserInfo info;
	std::map<int, int> mesh_to_acceleration_structure_index;
	int material_index_offset = 0;
	int texture_index_offset = 0;
public:
	RaytracingModelParser(const RaytracingModelParserInfo &info);

	MeshHandle createMesh(const ModelPrimitiveData& data, ModelInfo model_info) override;

	PipelineInstanceHandle createMaterial(const ModelMaterialData& materialData, ImageHandle* textures) override;

	ImageHandle createTexture(const ModelTextureData& data) override;

	void onStartParsingModel(HBE::Model *model) override;

	MeshAccelerationStructureHandle createMeshAccelerationStructure(Mesh &mesh, int mesh_index);

private:
	AccelerationStructureInstance createAccelerationStructureInstance(ModelNode &node, int primitive) override;

};
