#version 450
// Required for Vulkan shaders to work
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 modelViewProjection;
    mat4 model;
    mat4 view;
    mat4 normal;

    vec4 lightPos;
    vec4 viewPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;

layout(location = 0) out VertexShaderOutput {
    vec3 fragPosition;
    vec2 texCoords;

    vec3 tangentLightPos;
    vec3 tangentViewPos;
    vec3 tangentFragPos;

    vec3 test;
} vso;
/*
layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragLightDir;
*/


out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = ubo.modelViewProjection * vec4(inPosition, 1);
    gl_Position.y = -gl_Position.y;

    mat3 normalMat = mat3(ubo.normal);

    vec3 tangent = normalize(normalMat * inTangent);
    vec3 bitangent = normalize(normalMat * inBitangent);
    vec3 normal = normalize(normalMat * inNormal);

    mat3 TBN = transpose(mat3(tangent, bitangent, normal));

    vso.fragPosition = vec3(ubo.model * vec4(inPosition, 1));
    vso.texCoords = inTexCoord;

    vso.tangentLightPos = TBN * ubo.lightPos.xyz;
    vso.tangentViewPos = TBN * ubo.viewPos.xyz;
    vso.tangentFragPos = TBN * vso.fragPosition;
}