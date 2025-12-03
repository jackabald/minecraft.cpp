#pragma once

// Block types - pretty straightforward
// AIR is empty, everything else is solid

#include <cstdint>

enum class BlockType : uint8_t {
    AIR = 0,   // empty space
    STONE = 1, // solid rock
    DIRT = 2,  // dirt block
    GRASS = 3, // grass top
    COUNT = 4  // total types
};

// Check if block is solid (renders and blocks movement)
inline bool isSolid(BlockType type) {
    return type != BlockType::AIR;
}
