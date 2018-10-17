#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform UniformBufferObject {
    vec3 lightColor;
    vec3 ambientColor;
} ubo;
layout(binding = 2) uniform sampler2D diffuseSampler;
layout(binding = 3) uniform sampler2D normalSampler;

layout(location = 0) in FragmentShaderInput {
    vec3 fragPos;
    vec2 texCoords;

    vec3 tangentLightPos;
    vec3 tangentViewPos;
    vec3 tangentFragPos;
} fsi;

layout(location = 0) out vec4 outColor;

const float SPECULAR_POWER = 64.0;
const float SPECULAR_MODIFIER = 0.4;

void main() {
    // Get [0; 1] normals
    vec3 normal = texture(normalSampler, fsi.texCoords).rgb;
    // Translate to [-1; 1] normals
    normal = normalize(normal * 2.0 - 1.0);

    vec3 color = texture(diffuseSampler, fsi.texCoords).rgb;

    vec3 lightDir = normalize(fsi.tangentLightPos - fsi.tangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);

    vec3 viewDir = normalize(fsi.tangentViewPos - fsi.tangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), SPECULAR_POWER) * SPECULAR_MODIFIER;

    vec3 light = ubo.ambientColor + (diff + spec) * ubo.lightColor;

    outColor = vec4(light * color, 1);
}