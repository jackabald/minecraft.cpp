#include "world/World.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <glm/glm.hpp>

World::World(int seed) : m_seed(seed) {
    // Load initial chunks at origin
    for (int x = -2; x <= 2; ++x) {
        for (int z = -2; z <= 2; ++z) {
            getOrCreateChunk(x, z);
        }
    }
}

std::shared_ptr<Chunk> World::getOrCreateChunk(int chunkX, int chunkZ) {
    int64_t key = encodeChunkKey(chunkX, chunkZ);
    
    auto it = m_chunks.find(key);
    if (it != m_chunks.end()) {
        return it->second;
    }

    // Create new chunk and generate terrain
    auto chunk = std::make_shared<Chunk>(chunkX, chunkZ);
    generateTerrain(chunk);
    chunk->generateMesh();

    m_chunks[key] = chunk;
    return chunk;
}

float World::getNoise(float x, float z) const {
    // Simple noise function based on sine waves and the seed
    // Could use a proper Perlin noise library
    float n = std::sin(x * 0.1f + m_seed) * std::cos(z * 0.1f + m_seed);
    n += std::sin((x + z) * 0.05f) * std::cos((x - z) * 0.05f);
    return (n + 2.0f) / 4.0f; // Normalize to roughly 0-1
}

void World::generateTerrain(std::shared_ptr<Chunk> chunk) {
    int chunkX = chunk->getChunkX();
    int chunkZ = chunk->getChunkZ();

    for (int lx = 0; lx < Chunk::WIDTH; ++lx) {
        for (int lz = 0; lz < Chunk::DEPTH; ++lz) {
            // World coordinates
            float wx = chunkX * Chunk::WIDTH + lx;
            float wz = chunkZ * Chunk::DEPTH + lz;

            // Generate height based on noise (wider range for more variety)
            float noiseVal = getNoise(wx, wz);
            int maxHeight = 20 + (int)(noiseVal * 60.0f); // Height range: 20-80
            maxHeight = glm::clamp(maxHeight, 5, Chunk::HEIGHT - 1);

            for (int y = 0; y < maxHeight; ++y) {
                BlockType blockType;

                // More varied layering based on depth
                if (y < 5) {
                    // Deep bedrock-like stone
                    blockType = BlockType::STONE;
                } else if (y < maxHeight - 3) {
                    // Middle stone/dirt
                    blockType = BlockType::STONE;
                } else if (y < maxHeight - 1) {
                    // Dirt layer
                    blockType = BlockType::DIRT;
                } else {
                    // Grass on top
                    blockType = BlockType::GRASS;
                }

                chunk->setBlock(lx, y, lz, blockType);
            }

            // Fill rest with air
            for (int y = maxHeight; y < Chunk::HEIGHT; ++y) {
                chunk->setBlock(lx, y, lz, BlockType::AIR);
            }
        }
    }
}

void World::update(const glm::vec3& cameraPos, int renderDistance) {
    // Determine which chunk the camera is in
    int cameraChunkX = (int)std::floor(cameraPos.x / Chunk::WIDTH);
    int cameraChunkZ = (int)std::floor(cameraPos.z / Chunk::DEPTH);

    // Only update if camera moved to a different chunk
    if (cameraChunkX == m_centerChunkX && cameraChunkZ == m_centerChunkZ) {
        return;
    }

    m_centerChunkX = cameraChunkX;
    m_centerChunkZ = cameraChunkZ;

    // Load chunks around the camera
    for (int x = cameraChunkX - renderDistance; x <= cameraChunkX + renderDistance; ++x) {
        for (int z = cameraChunkZ - renderDistance; z <= cameraChunkZ + renderDistance; ++z) {
            getOrCreateChunk(x, z);
        }
    }

    // TODO: Unload distant chunks
    // For now, we'll keep all loaded chunks.
}

void World::render(unsigned int shaderProgram) const {
    glm::mat4 identity = glm::mat4(1.0f);
    
    for (const auto& [key, chunk] : m_chunks) {
        if (chunk && chunk->hasMesh()) {
            chunk->render(shaderProgram, identity);
        }
    }
}
