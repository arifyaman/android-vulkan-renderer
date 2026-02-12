#version 450
#extension GL_EXT_nonuniform_qualifier : enable

// Bindless textures - unbounded array
layout(binding = 1) uniform sampler2D textures[];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) flat in int fragTexIndex;

layout(location = 0) out vec4 outColor;

void main() {
    // Use nonuniform qualifier for dynamic indexing
    outColor = texture(textures[nonuniformEXT(fragTexIndex)], fragTexCoord);
}
