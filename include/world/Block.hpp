#pragma once

#include <cstdint>
#include <array>

enum class BlockType : uint8_t {
    AIR = 0,
    STONE,
    DIRT,
    GRASS,
    SAND,
    WATER,
    WOOD,
    LEAVES,
    COUNT
};

struct BlockData {
    bool isSolid;
    bool isTransparent;
    float color[3];
};

namespace BlockDB {
    inline const BlockData& get(BlockType type) {
        static const std::array<BlockData, static_cast<size_t>(BlockType::COUNT)> data = {{
            { false, true,  {0.0f,  0.0f,  0.0f } },  // AIR
            { true,  false, {0.5f,  0.5f,  0.5f } },  // STONE
            { true,  false, {0.55f, 0.36f, 0.23f} },  // DIRT
            { true,  false, {0.2f,  0.8f,  0.2f } },  // GRASS (top color)
            { true,  false, {0.9f,  0.85f, 0.6f } },  // SAND
            { false, true,  {0.2f,  0.4f,  0.8f } },  // WATER
            { true,  false, {0.55f, 0.35f, 0.15f} },  // WOOD
            { true,  false, {0.1f,  0.6f,  0.1f } },  // LEAVES
        }};
        return data[static_cast<size_t>(type)];
    }
}

inline bool isSolid(BlockType type) {
    return BlockDB::get(type).isSolid;
}

inline bool isTransparent(BlockType type) {
    return BlockDB::get(type).isTransparent;
}
