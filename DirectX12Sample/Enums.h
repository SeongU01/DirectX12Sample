#pragma once
enum class FeatureLevel { LEVEL_11_0, LEVEL_12_0, LEVEL_12_1 };
enum class CommandType { DIRECT, BUNDLE, COMPUTE};
enum CommandQueueType
{
    GRAPHICS_QUEUE,
    COMPUTE_QUEUE,
    COPY_QUEUE,
    COMMAND_QUEUE_END
};
