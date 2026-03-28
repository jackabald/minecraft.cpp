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

    // Face color for a block — grass gets different colors per face
    auto getFaceColor = [](BlockType type, int face) -> std::array<float, 3> {
        const auto& data = BlockDB::get(type);
        if (type == BlockType::GRASS) {
            if (face == 5) return {0.2f, 0.8f, 0.2f};       // top: green
            if (face == 4) return {0.55f, 0.36f, 0.23f};     // bottom: dirt
            return {0.45f, 0.6f, 0.2f};                       // sides: greenish-brown
        }
        return {data.color[0], data.color[1], data.color[2]};
    };

    // Helper to emit a face quad (4 verts, 6 indices)
    auto addFace = [&](float x0, float y0, float z0,
                       float x1, float y1, float z1,
                       float x2, float y2, float z2,
                       float x3, float y3, float z3,
                       const std::array<float, 3>& color) {
        unsigned int base = vertices.size() / 6;
        // 4 vertices
        float verts[] = { x0,y0,z0, x1,y1,z1, x2,y2,z2, x3,y3,z3 };
        for (int i = 0; i < 4; ++i) {
            vertices.push_back(verts[i*3]);
            vertices.push_back(verts[i*3+1]);
            vertices.push_back(verts[i*3+2]);
            vertices.push_back(color[0]);
            vertices.push_back(color[1]);
            vertices.push_back(color[2]);
        }
        // 2 triangles (CCW winding)
        indices.push_back(base);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
        indices.push_back(base);
    };

    // Iterate through all blocks — only emit faces adjacent to non-solid blocks
    for (int y = 0; y < HEIGHT; ++y) {
        for (int z = 0; z < DEPTH; ++z) {
            for (int x = 0; x < WIDTH; ++x) {
                BlockType blockType = getBlock(x, y, z);
                if (!isSolid(blockType)) continue;

                float wx = static_cast<float>(m_chunkX * WIDTH + x);
                float wy = static_cast<float>(y);
                float wz = static_cast<float>(m_chunkZ * DEPTH + z);

                // -X face
                if (!isNeighborSolid(x - 1, y, z)) {
                    auto c = getFaceColor(blockType, 0);
                    addFace(wx-0.5f, wy-0.5f, wz-0.5f,
                            wx-0.5f, wy-0.5f, wz+0.5f,
                            wx-0.5f, wy+0.5f, wz+0.5f,
                            wx-0.5f, wy+0.5f, wz-0.5f, c);
                }
                // +X face
                if (!isNeighborSolid(x + 1, y, z)) {
                    auto c = getFaceColor(blockType, 1);
                    addFace(wx+0.5f, wy-0.5f, wz+0.5f,
                            wx+0.5f, wy-0.5f, wz-0.5f,
                            wx+0.5f, wy+0.5f, wz-0.5f,
                            wx+0.5f, wy+0.5f, wz+0.5f, c);
                }
                // -Z face
                if (!isNeighborSolid(x, y, z - 1)) {
                    auto c = getFaceColor(blockType, 2);
                    addFace(wx+0.5f, wy-0.5f, wz-0.5f,
                            wx-0.5f, wy-0.5f, wz-0.5f,
                            wx-0.5f, wy+0.5f, wz-0.5f,
                            wx+0.5f, wy+0.5f, wz-0.5f, c);
                }
                // +Z face
                if (!isNeighborSolid(x, y, z + 1)) {
                    auto c = getFaceColor(blockType, 3);
                    addFace(wx-0.5f, wy-0.5f, wz+0.5f,
                            wx+0.5f, wy-0.5f, wz+0.5f,
                            wx+0.5f, wy+0.5f, wz+0.5f,
                            wx-0.5f, wy+0.5f, wz+0.5f, c);
                }
                // -Y face (bottom)
                if (!isNeighborSolid(x, y - 1, z)) {
                    auto c = getFaceColor(blockType, 4);
                    addFace(wx-0.5f, wy-0.5f, wz+0.5f,
                            wx-0.5f, wy-0.5f, wz-0.5f,
                            wx+0.5f, wy-0.5f, wz-0.5f,
                            wx+0.5f, wy-0.5f, wz+0.5f, c);
                }
                // +Y face (top)
                if (!isNeighborSolid(x, y + 1, z)) {
                    auto c = getFaceColor(blockType, 5);
                    addFace(wx-0.5f, wy+0.5f, wz-0.5f,
                            wx+0.5f, wy+0.5f, wz-0.5f,
                            wx+0.5f, wy+0.5f, wz+0.5f,
                            wx-0.5f, wy+0.5f, wz+0.5f, c);
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
