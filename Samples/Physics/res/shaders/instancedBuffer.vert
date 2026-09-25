#version 450
layout(location = 0) in vec3 inPosition;
layout (location = 0) out flat int fragInstanceID;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
} ubo;

layout(binding = 1, std430) readonly buffer inInstanceTransforms{
    mat4 transforms[];
} transformBuffer;


void main() {
    fragInstanceID = gl_InstanceIndex;
    gl_Position = ubo.projection * ubo.view * transformBuffer.transforms[gl_InstanceIndex] * vec4(inPosition, 1.0);
}