// Bindless Slang shader for Vulkan
// Compile with: slangc shader.slang -target spirv -entry vertexMain -stage vertex -entry fragmentMain -stage fragment -o shader.spv
// Requires VK_EXT_descriptor_indexing extension

struct UniformBufferObject
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};

[[vk::binding(0, 0)]]
ConstantBuffer<UniformBufferObject> ubo;

// Bindless textures - unbounded array
// This allows thousands of textures to be bound
[[vk::binding(1, 0)]]
Sampler2D textures[];

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
    [[vk::location(2)]] nointerpolation int texIndex : TEXINDEX;
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
    // Bindless texture sampling with dynamic indexing
    output.color = textures[NonUniformResourceIndex(input.texIndex)].Sample(input.texCoord);
    return output;
}
