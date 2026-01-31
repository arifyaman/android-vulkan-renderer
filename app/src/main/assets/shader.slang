// Slang shader for Vulkan
// Compile with: slangc shader.slang -target spirv -entry vertexMain -stage vertex -entry fragmentMain -stage fragment -o shader.spv
// Or separately: slangc shader.slang -target spirv -entry vertexMain -stage vertex -o shader.vert.spv
//                slangc shader.slang -target spirv -entry fragmentMain -stage fragment -o shader.frag.spv

// Maximum number of textures (for Phase 1: Texture Array)
// For Phase 2 (Bindless): This can be increased significantly
#ifndef MAX_MATERIALS
#define MAX_MATERIALS 65536
#endif

struct UniformBufferObject
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};

[[vk::binding(0, 0)]]
ConstantBuffer<UniformBufferObject> ubo;

// Phase 1: Array of textures (Texture2DArray approach)
// Phase 2: Will convert to bindless descriptor array
[[vk::binding(1, 0)]]
Texture2D textures[MAX_MATERIALS];
SamplerState texSampler;

// Vertex shader input
struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float3 color : COLOR;
    [[vk::location(2)]] float2 texCoord : TEXCOORD;
    [[vk::location(3)]] int texIndex : TEXINDEX;
};

// Vertex shader output / Fragment shader input
struct VertexOutput
{
    float4 position : SV_Position;
    [[vk::location(0)]] float3 color : COLOR;
    [[vk::location(1)]] float2 texCoord : TEXCOORD;
    [[vk::location(2)]] int texIndex : TEXINDEX;
};

// Vertex shader entry point
[shader("vertex")]
VertexOutput vertexMain(VertexInput input)
{
    VertexOutput output;

    float4 worldPos = mul(ubo.model, float4(input.position, 1.0));
    float4 viewPos = mul(ubo.view, worldPos);
    output.position = mul(ubo.proj, viewPos);

    output.color = input.color;
    output.texCoord = input.texCoord;
    output.texIndex = input.texIndex;

    return output;
}

// Fragment shader output
struct FragmentOutput
{
    [[vk::location(0)]] float4 color : SV_Target;
};

// Fragment shader entry point
[shader("fragment")]
FragmentOutput fragmentMain(VertexOutput input)
{
    FragmentOutput output;
    // Phase 1: Sample from texture array based on index
    // Phase 2: Will use non-uniform index for bindless textures
    output.color = textures[input.texIndex].Sample(texSampler, input.texCoord);
    return output;
}
