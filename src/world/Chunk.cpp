#include "world/Chunk.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

Chunk::Chunk(int chunkX, int chunkZ)
    : m_chunkX(chunkX), m_chunkZ(chunkZ) {
    // Initialize block storage with AIR
    m_blocks.resize(VOLUME, BlockType::AIR);
}

Chunk::~Chunk() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
}

BlockType Chunk::getBlock(int x, int y, int z) const {
    if (!isInBounds(x, y, z)) return BlockType::AIR;
    return m_blocks[getBlockIndex(x, y, z)];
}

void Chunk::setBlock(int x, int y, int z, BlockType type) {
    if (!isInBounds(x, y, z)) return;
    m_blocks[getBlockIndex(x, y, z)] = type;
}

bool Chunk::isNeighborSolid(int x, int y, int z) const {
    if (!isInBounds(x, y, z)) {
        // Blocks outside this chunk are considered AIR
        // This prevents holes at chunk boundaries and is conservative
        // (renders more faces, but ensures no gaps in the world)
        return false;
    }
    return isSolid(getBlock(x, y, z));
}

bool Chunk::isBlockVisible(int x, int y, int z) const {
    if (!isSolid(getBlock(x, y, z))) return false;

    // A block is visible if at least one neighbor is NOT solid (empty/air)
    // Check all 6 faces
    if (!isNeighborSolid(x - 1, y, z)) return true;
    if (!isNeighborSolid(x + 1, y, z)) return true;
    if (!isNeighborSolid(x, y - 1, z)) return true;
    if (!isNeighborSolid(x, y + 1, z)) return true;
    if (!isNeighborSolid(x, y, z - 1)) return true;
    if (!isNeighborSolid(x, y, z + 1)) return true;

    return false;
}

void Chunk::generateMesh() {
    // Clean up old mesh if it exists
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);

    std::vector<float> vertices; // Position (x,y,z) + Color (r,g,b) = 6 floats per vertex
    std::vector<unsigned int> indices;

    // Helper lambda to get color for a block type
    auto getBlockColor = [](BlockType type) -> std::array<float, 3> {
        switch (type) {
            case BlockType::STONE: return {0.5f, 0.5f, 0.5f};   // Gray
            case BlockType::DIRT:  return {0.55f, 0.36f, 0.23f}; // Brown
            case BlockType::GRASS: return {0.2f, 0.8f, 0.2f};    // Green
            case BlockType::AIR:
            default:               return {1.0f, 1.0f, 1.0f};    // White (shouldn't render)
        }
    };

    // Iterate through all blocks in the chunk
    for (int y = 0; y < HEIGHT; ++y) {
        for (int z = 0; z < DEPTH; ++z) {
            for (int x = 0; x < WIDTH; ++x) {
                if (!isBlockVisible(x, y, z)) continue;

                BlockType blockType = getBlock(x, y, z);
                auto color = getBlockColor(blockType);

                // Get world position of this block's corner
                float wx = m_chunkX * WIDTH + x;
                float wy = y;
                float wz = m_chunkZ * DEPTH + z;

                unsigned int baseIndex = vertices.size() / 6; // 6 floats per vertex now

                // Add cube vertices (8 vertices per cube) with position + color
                // Front face (z+)
                for (int i = 0; i < 4; ++i) {
                    vertices.push_back((i == 0 || i == 3) ? wx - 0.5f : wx + 0.5f);
                    vertices.push_back((i < 2) ? wy - 0.5f : wy + 0.5f);
                    vertices.push_back(wz + 0.5f);
                    vertices.push_back(color[0]);
                    vertices.push_back(color[1]);
                    vertices.push_back(color[2]);
                }
                // Back face (z-)
                for (int i = 0; i < 4; ++i) {
                    vertices.push_back((i == 0 || i == 3) ? wx - 0.5f : wx + 0.5f);
                    vertices.push_back((i < 2) ? wy - 0.5f : wy + 0.5f);
                    vertices.push_back(wz - 0.5f);
                    vertices.push_back(color[0]);
                    vertices.push_back(color[1]);
                    vertices.push_back(color[2]);
                }
                // Left face (x-)
                for (int i = 0; i < 4; ++i) {
                    vertices.push_back(wx - 0.5f);
                    vertices.push_back((i < 2) ? wy - 0.5f : wy + 0.5f);
                    vertices.push_back((i == 0 || i == 3) ? wz - 0.5f : wz + 0.5f);
                    vertices.push_back(color[0]);
                    vertices.push_back(color[1]);
                    vertices.push_back(color[2]);
                }
                // Right face (x+)
                for (int i = 0; i < 4; ++i) {
                    vertices.push_back(wx + 0.5f);
                    vertices.push_back((i < 2) ? wy - 0.5f : wy + 0.5f);
                    vertices.push_back((i == 0 || i == 3) ? wz - 0.5f : wz + 0.5f);
                    vertices.push_back(color[0]);
                    vertices.push_back(color[1]);
                    vertices.push_back(color[2]);
                }
                // Bottom face (y-)
                for (int i = 0; i < 4; ++i) {
                    vertices.push_back((i == 0 || i == 3) ? wx - 0.5f : wx + 0.5f);
                    vertices.push_back(wy - 0.5f);
                    vertices.push_back((i < 2) ? wz - 0.5f : wz + 0.5f);
                    vertices.push_back(color[0]);
                    vertices.push_back(color[1]);
                    vertices.push_back(color[2]);
                }
                // Top face (y+)
                for (int i = 0; i < 4; ++i) {
                    vertices.push_back((i == 0 || i == 3) ? wx - 0.5f : wx + 0.5f);
                    vertices.push_back(wy + 0.5f);
                    vertices.push_back((i < 2) ? wz - 0.5f : wz + 0.5f);
                    vertices.push_back(color[0]);
                    vertices.push_back(color[1]);
                    vertices.push_back(color[2]);
                }

                // Add indices for 6 faces (2 triangles per face, 3 indices each)
                for (int face = 0; face < 6; ++face) {
                    unsigned int faceStart = baseIndex + face * 4;
                    indices.push_back(faceStart);
                    indices.push_back(faceStart + 1);
                    indices.push_back(faceStart + 2);
                    indices.push_back(faceStart + 2);
                    indices.push_back(faceStart + 3);
                    indices.push_back(faceStart);
                }
            }
        }
    }

    m_indexCount = indices.size();

    // If no visible blocks, skip GPU upload
    if (m_indexCount == 0) {
        return;
    }

    // Upload to GPU
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Vertex position attribute (location 0): 3 floats, offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex color attribute (location 1): 3 floats, offset 3*sizeof(float)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Chunk::render(unsigned int shaderProgram, const glm::mat4& modelMatrix) const {
    if (m_indexCount == 0) return; // No mesh to render

    glUseProgram(shaderProgram);

    // Set model matrix uniform
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
}

glm::vec3 Chunk::getWorldPosition() const {
    return glm::vec3(m_chunkX * WIDTH, 0.0f, m_chunkZ * DEPTH);
}
