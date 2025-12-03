#pragma once

// Manages all chunks and the world's terrain
// Generates terrain using perlin noise and handles chunk loading/unloading
// based on where the camera is looking

#include "world/Chunk.hpp"
#include <glm/glm.hpp>
#include <map>
#include <memory>

class World {
public:
    // Create world with optional seed (default seed works fine)
    explicit World(int seed = 12345);

    // Update which chunks to keep loaded - call every frame
    // Loads chunks near camera, unloads distant ones
    void update(const glm::vec3& cameraPos, int renderDistance = 4);

    // Render all the loaded chunks
    void render(unsigned int shaderProgram) const;

    // Get or create chunk at these coordinates
    std::shared_ptr<Chunk> getOrCreateChunk(int chunkX, int chunkZ);

private:
    // Populate a chunk with terrain blocks
    void generateTerrain(std::shared_ptr<Chunk> chunk);

    // Simple noise function for terrain generation
    // (simplified for demo - would use proper perlin in production)
    float getNoise(float x, float z) const;

    int m_seed; // for reproducible terrain generation

    // chunks stored by encoded coords (chunkX, chunkZ)
    std::map<int64_t, std::shared_ptr<Chunk>> m_chunks;

    int m_centerChunkX = 0;
    int m_centerChunkZ = 0;

    // Pack chunk coords into single key for the map
    static int64_t encodeChunkKey(int chunkX, int chunkZ) {
        return ((int64_t)chunkX << 32) | (uint32_t)chunkZ;
    }
};
