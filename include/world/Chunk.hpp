#pragma once

// Chunk - 16x16x256 section of blocks
// Stores block data and generates a mesh for rendering
// Basically all visible faces get baked into one VAO/VBO

#include "world/Block.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

class Chunk {
public:
    static constexpr int WIDTH = 16;   // x dimension
    static constexpr int DEPTH = 16;   // z dimension
    static constexpr int HEIGHT = 256; // y dimension (world height)
    static constexpr int VOLUME = WIDTH * DEPTH * HEIGHT;

    // Create a chunk at world chunk coords (chunkX, chunkZ)
    Chunk(int chunkX, int chunkZ);
    ~Chunk();

    // Get block at local position (0-15, 0-255, 0-15)
    // Returns AIR if out of bounds
    BlockType getBlock(int x, int y, int z) const;

    // Set block at local position
    void setBlock(int x, int y, int z, BlockType type);

    // Generate mesh from current blocks
    // Iterates through blocks, builds vertices for visible faces, uploads to GPU
    void generateMesh();

    // Draw this chunk with the given shader
    void render(unsigned int shaderProgram, const glm::mat4& modelMatrix) const;

    // Get world position (bottom-left corner of chunk)
    glm::vec3 getWorldPosition() const;

    // Getters
    int getChunkX() const { return m_chunkX; }
    int getChunkZ() const { return m_chunkZ; }
    
    // Check if we have a valid mesh to render
    bool hasMesh() const { return m_indexCount > 0; }

private:
    // Check if block has at least one exposed face
    bool isBlockVisible(int x, int y, int z) const;

    // Check if neighbor is solid or out of bounds
    // Used to determine if a face should be rendered
    bool isNeighborSolid(int x, int y, int z) const;

    // Chunk coords in world space
    int m_chunkX;
    int m_chunkZ;

    // 16 * 16 * 256 = 65,536 blocks per chunk
    std::vector<BlockType> m_blocks;

    // GPU stuff
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;
    unsigned int m_indexCount = 0; // how many indices in the mesh

    // Get linear index for a block in the array
    inline int getBlockIndex(int x, int y, int z) const {
        return y * (WIDTH * DEPTH) + z * WIDTH + x;
    }

    // Check if coords are in bounds
    inline bool isInBounds(int x, int y, int z) const {
        return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && z >= 0 && z < DEPTH;
    }
};
