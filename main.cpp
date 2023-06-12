/**
* Author: JumboDS64
* Assignment: Simple 2D Scene
* Date due: 2023-06-11, 8:17pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix1, modelMatrix2, projectionMatrix;
glm::vec3 position1, position2;
float rotation1, rotation2;
glm::vec3 scale1, scale2;
float vertices1[] = { 0.5f, 0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f,  -0.5f, -0.5f,  0.5f, -0.5f,  0.5f, 0.5f };
float vertices2[] = { 0.2f, 0.2f,  -0.1f, 0.1f,  -0.2f, -0.2f,  -0.2f, -0.2f,  0.1f, -0.1f,  0.2f, 0.2f };//{ 0.0f, 0.0f, 0.0f, 0.2f, 0.2f, 0.1f, 0.2f, -0.2f, -0.2f, -0.2f, -0.2f, 0.1f, 0.0f, 0.2f };
float uvs1[] = { 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f };
float uvs2[] = { 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f };
char texture1_path[] = "spr1.png";
GLuint texture1;
char texture2_path[] = "spr2.png";
GLuint texture2;
float time_c;
float time_p;
float delta;
float time_total;
const glm::vec3 vec_out = glm::vec3(0,0,1);

GLuint load_texture(const char* filepath) {
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct.\n";
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Triangle!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    position1 = glm::vec3(0);
    rotation1 = 0;
    scale1 = glm::vec3(1);
    texture1 = load_texture(texture1_path);
    modelMatrix1 = glm::mat4(1.0f);
    position2 = glm::vec3(0);
    rotation2 = 0;
    scale2 = glm::vec3(1);
    time_total = 0;
    texture2 = load_texture(texture2_path);
    modelMatrix2 = glm::mat4(1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.0f, 0.1f, 0.05f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

void Update() {
    time_c = (float)SDL_GetTicks() / 1000.;
    delta = time_c - time_p;
    time_p = time_c;
    time_total += delta;

    position1.x = sin(time_total) * 2;
    position1.y = cos(time_total) * 2;
    rotation1 += 0.5 * delta;
    scale1.x = 1.2 + sin(time_total * 8) * 0.1;
    scale1.y = 1.2 - sin(time_total * 8) * 0.1;
    float sp = 4;
    //sburb spirograph
    float p2x = cos(time_total*sp) * (17.0f/6.0f) - (7.0f/6.0f) * cos((time_total*sp*(17.0f/6.0f)) / (7.0f/6.0f));
    float p2y = sin(time_total*sp) * (17.0f/6.0f) - (7.0f/6.0f) * sin((time_total*sp*(17.0f/6.0f)) / (7.0f/6.0f));
    position2.x = position1.x + p2x*0.3;
    position2.y = position1.y + p2y*0.3;
    rotation2 -= 2 * delta;
    scale2.x = sqrt(pow(p2x,2) + pow(p2y,2)) * 0.5 - 0.3;
    scale2.y = scale2.x;

    modelMatrix1 = glm::translate(glm::mat4(1.0f), position1);
    modelMatrix1 = glm::rotate(modelMatrix1, rotation1, vec_out);
    modelMatrix1 = glm::scale(modelMatrix1, scale1);
    modelMatrix2 = glm::translate(glm::mat4(1.0f), position2);
    modelMatrix2 = glm::rotate(modelMatrix2, rotation2, vec_out);
    modelMatrix2 = glm::scale(modelMatrix2, scale2);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, uvs2);
    glEnableVertexAttribArray(program.positionAttribute);
    glEnableVertexAttribArray(program.texCoordAttribute);
    program.SetModelMatrix(modelMatrix2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, uvs1);
    glEnableVertexAttribArray(program.positionAttribute);
    glEnableVertexAttribArray(program.texCoordAttribute);
    program.SetModelMatrix(modelMatrix1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}