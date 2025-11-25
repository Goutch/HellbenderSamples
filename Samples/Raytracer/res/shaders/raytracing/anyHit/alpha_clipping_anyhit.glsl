#version 460
#extension GL_EXT_ray_tracing: enable
#extension GL_EXT_nonuniform_qualifier: enable
#extension GL_GOOGLE_include_directive : require
#include "../common.glsl"
#include "../mesh.glsl"

void main()
{
    InstanceInfo instance_info = instances.infos[gl_InstanceID];
    VertexData vertexData = getInterpolatedVertexData(instance_info);

    vec2 uvs = vertexData.tex_coords;
    vec3 normal = vertexData.normal;

    MaterialData material = materials.materials[instance_info.material_index];
    int albedo_index = material.albedo_index;

    if(material.albedo_index>0 && texture(textures[nonuniformEXT(material.albedo_index)], uvs).a < 0.9)
    {
        ignoreIntersectionEXT;
    }
    ignoreIntersectionEXT;
}