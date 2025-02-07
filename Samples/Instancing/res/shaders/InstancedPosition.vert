#version 450
//-----------------------------inputs---------------------------------
//vertices
layout(location = 0) in vec3 inPosition;

//instanced
layout(location = 1) in mat4 inInstanceTransforms;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
} ubo;
layout (location = 0) out flat int fragInstanceID;
void main() {
    fragInstanceID = gl_InstanceIndex;
    gl_Position = ubo.projection * ubo.view * inInstanceTransforms * vec4(inPosition, 1.0);
}