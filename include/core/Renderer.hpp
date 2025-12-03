#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void init(int width, int height);
    void clear();
    void drawCube(glm::vec3 position);
    void setViewMatrix(glm::mat4 view);
    void setProjectionMatrix(glm::mat4 projection);
    
    // Get the shader program (useful for external rendering like chunks)
    GLuint getShaderProgram() const { return shaderProgram; }

private:
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram;
    GLint viewLoc, projLoc, modelLoc;

    void setupCubeVertices();
    GLuint compileShader(const std::string& source, GLenum shaderType);
    GLuint createShaderProgram();
};
