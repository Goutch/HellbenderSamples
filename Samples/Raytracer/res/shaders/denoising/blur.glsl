#define RANGE 8
#define LAYER_COUNT 8
#include "../raytracing/data_structures.glsl"

layout (binding = 0, set = 0) uniform FrameUBO {
    Frame data;
} frame;

layout(binding = 1, set = 0, r32f) writeonly uniform image2D outputImage;
layout(binding = 2, set = 0, rgba32f) uniform readonly image2D inputImage;

layout (binding = 3, set = 0, rgba32f) uniform readonly image2D historyNormalDepth[HISTORY_COUNT];

float gaussianWeight(int x, float sigma) {
    return exp(-(x * x) / (2.0 * sigma * sigma));
}
shared vec4 samples[RANGE* LAYER_COUNT*3];
shared vec4 normalDepths[RANGE * LAYER_COUNT*3];
void blur(int center)
{
    float totalWeight = 1.0;
    vec4 total = samples[center];
    vec4 center_normal_depth = normalDepths[center];
    for (int i =1; i < RANGE; ++i) {
        int coord = center - i;
        vec4 other_normal_depth = normalDepths[coord];
        if (dot(center_normal_depth.xyz, other_normal_depth.xyz)<0.85)
        {
            break;
        }
        if (abs(center_normal_depth.w - other_normal_depth.w)>  mix(0.05, 0.25, center_normal_depth.w/100.0))
        {
            break;
        }
        float weight = gaussianWeight(i, RANGE*2);

        total += samples[coord] * weight;
        totalWeight += weight;
    }
    for (int i =1; i < RANGE; ++i) {
        int coord = center + i;
        vec4 other_normal_depth = normalDepths[coord];
        if (dot(center_normal_depth.xyz, other_normal_depth.xyz)<0.85)
        {
            break;
        }
        if (abs(center_normal_depth.w - other_normal_depth.w)>  mix(0.05, 0.25, center_normal_depth.w/100.0))
        {
            break;
        }
        float weight = gaussianWeight(i, 16.0);

        total += samples[coord] * weight;
        totalWeight += weight;
    }
    total /= totalWeight;
    imageStore(outputImage, ivec2(gl_GlobalInvocationID.xy), total);
}
