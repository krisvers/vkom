#define groupDimensions uint3(1, 1, 1)
#define dispatchDimensions uint3(256, 1, 1)

struct PushConstants {
    uint64_t buffer;
};

[[vk::push_constant]]
PushConstants pushConstants;

[[vk::binding(0, 0)]]
RWStructuredBuffer<uint32_t> ssbo;

[numthreads(1, 1, 1)]
void main(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint3 threadCoordinateID : SV_DispatchThreadID) {
    uint id = groupThreadID.x + groupDimensions.x * (groupThreadID.y + groupDimensions.y * (groupThreadID.z + groupDimensions.z * (groupID.x + dispatchDimensions.x * (groupID.y + dispatchDimensions.y * groupID.z))));
    if (id > 128) {
        vk::RawBufferStore<uint>(pushConstants.buffer + id * 4, id);
    } else {
        ssbo[id] = id;
    }
}
