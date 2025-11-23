#version 450

//----------------------outputs----------------------
layout(location = 0) out vec4 outColor;


//----------------------uniforms----------------------

layout(push_constant) uniform Constants
{
	mat4 transform;
	vec4 color;
} constants;

void main() {
	outColor = constants.color;
}