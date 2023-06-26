/**
* Author: JumboDS64
* Assignment: Pong Clone
* Date due: 2023-06-25, 11:55pm
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
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 scale1, scale2;
float time_c;
float time_p;
float delta;
float time_total;
const glm::vec3 vec_out = glm::vec3(0,0,1);

glm::mat4 paddle1_modelMatrix;
glm::vec3 paddle1_position;
float paddle1_vertices[] = { -0.5f, 0.5f,  0.5f, 0.5f,  0.5f, -0.5f,  0.5f, -0.5f,  -0.5f, -0.5f,  -0.5f, 0.5f };
float paddle1_uvs[] = { 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f };
glm::vec3 paddle1_scale;
char paddle1_texture_path[] = "paddle.png";
GLuint paddle1_texture;

glm::mat4 paddle2_modelMatrix;
glm::vec3 paddle2_position;
float paddle2_vertices[] = { -0.5f, 0.5f,  0.5f, 0.5f,  0.5f, -0.5f,  0.5f, -0.5f,  -0.5f, -0.5f,  -0.5f, 0.5f };
float paddle2_uvs[] = { 0.0f, 0.0f,  -1.0f, 0.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f };
glm::vec3 paddle2_scale;
char paddle2_texture_path[] = "paddle.png";
GLuint paddle2_texture;

glm::mat4 ball_modelMatrix;
glm::vec3 ball_position;
glm::vec3 ball_velocity;
float ball_vertices[] = { -0.5f, 0.5f,  0.5f, 0.5f,  0.5f, -0.5f,  0.5f, -0.5f,  -0.5f, -0.5f,  -0.5f, 0.5f };
float ball_uvs[] = { 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f };
glm::vec3 ball_scale;
float ball_rotation;
char ball_texture_path[] = "spr2.png";
GLuint ball_texture;

glm::mat4 wintext_modelMatrix;
glm::vec3 wintext_position;
float wintext_vertices[] = { -2.0f, 0.5f,  2.0f, 0.5f,  2.0f, -0.5f,  2.0f, -0.5f,  -2.0f, -0.5f,  -2.0f, 0.5f };
float wintext_uvs_1[] = { 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 0.5f,  1.0f, 0.5f,  0.0f, 0.5f,  0.0f, 0.0f };
float wintext_uvs_2[] = { 0.0f, 0.5f,  1.0f, 0.5f,  1.0f, 1.0f,  1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.5f };
char wintext_texture_path[] = "wintext.png";
GLuint wintext_texture;

float dim_x = 4.75;
float dim_x2 = 4.25;
float dim_y = 3.5;
float ball_speed = 1;
int score = 0;
int whowon = 0;
int paddle1_dir = 0;
int paddle2_dir = 0;
float paddle_width = 1;
int wintext_timer = 0;

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

    paddle1_position = glm::vec3(0);
    paddle1_position.x = -dim_x2;
    paddle1_scale = glm::vec3(1);
    paddle1_scale.x = 0.25;
    paddle1_texture = load_texture(paddle1_texture_path);
    paddle1_modelMatrix = glm::mat4(1.0f);

    paddle2_position = glm::vec3(0);
    paddle2_position.x = dim_x2;
    paddle2_scale = glm::vec3(1);
    paddle2_scale.x = 0.25;
    paddle2_texture = load_texture(paddle2_texture_path);
    paddle2_modelMatrix = glm::mat4(1.0f);

    ball_speed = 2;

    ball_position = glm::vec3(0);
    ball_velocity = glm::vec3(0);
    ball_velocity.x = ball_speed;
    ball_velocity.y = ball_speed;
    ball_rotation = 0;
    ball_scale = glm::vec3(0.5);
    ball_texture = load_texture(ball_texture_path);
    ball_modelMatrix = glm::mat4(1.0f);

    wintext_position = glm::vec3(0);
    wintext_position.x = dim_x2;
    wintext_texture = load_texture(wintext_texture_path);
    wintext_modelMatrix = glm::mat4(1.0f);

    time_total = 0;

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
        switch (event.type) {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_w:
                    paddle1_dir = 1;
                    break;
                case SDLK_s:
                    paddle1_dir = -1;
                    break;
                case SDLK_UP:
                    paddle2_dir = 1;
                    break;
                case SDLK_DOWN:
                    paddle2_dir = -1;
                    break;
            }

        default:
            break;
        }
    }
}

void Update() {
    time_c = (float)SDL_GetTicks() / 1000.;
    delta = time_c - time_p;
    time_p = time_c;
    time_total += delta;

    wintext_position.x = 0;
    if (wintext_timer > 0) {
        wintext_position.y = 0;
        wintext_timer -= 1*delta;
    }
    else {
        wintext_position.y = 100;
    }

    paddle1_position.y += 2 * ball_speed * paddle1_dir * delta;
    paddle2_position.y += 2 * ball_speed * paddle2_dir * delta;
    if (paddle1_position.y < -dim_y) paddle1_position.y = -dim_y;
    if (paddle1_position.y > dim_y) paddle1_position.y = dim_y;
    if (paddle2_position.y < -dim_y) paddle2_position.y = -dim_y;
    if (paddle2_position.y > dim_y) paddle2_position.y = dim_y;

    ball_position.x += ball_velocity.x*delta;
    ball_position.y += ball_velocity.y*delta;
    if (ball_position.y < -dim_y) {
        ball_position.y = -dim_y*2 - ball_position.y;
        ball_velocity.y = -ball_velocity.y;
    } else if (ball_position.y > dim_y) {
        ball_position.y = dim_y*2 - ball_position.y;
        ball_velocity.y = -ball_velocity.y;
    }
    if (ball_position.x < -dim_x2 && ball_position.y > paddle1_position.y - paddle_width && ball_position.y < paddle1_position.y + paddle_width) {
        ball_position.x = -dim_x2 * 2 - ball_position.x;
        ball_velocity.x = -ball_velocity.x;
    } else if (ball_position.x > dim_x2 && ball_position.y > paddle2_position.y - paddle_width && ball_position.y < paddle2_position.y + paddle_width) {
        ball_position.x = dim_x2 * 2 - ball_position.x;
        ball_velocity.x = -ball_velocity.x;
    }
    if (ball_position.x < -dim_x) {
        whowon = 1;
        score -= 1;
        ball_position.x = 0;
        ball_position.y = 0;
        ball_velocity.x = ball_speed; //winner serves
        ball_velocity.y = ball_speed;
        paddle1_position.y = 0;
        paddle2_position.y = 0;
        paddle1_dir = 0;
        paddle2_dir = 0;
        ball_speed = 1 + abs(score)*0.1; //consecutive wins raise the difficulty
        wintext_timer = 1000;
    } else if (ball_position.x > dim_x) {
        whowon = -1;
        score += 1;
        ball_position.x = 0;
        ball_position.y = 0;
        ball_velocity.x = -ball_speed; //winner serves
        ball_velocity.y = ball_speed;
        paddle1_position.y = 0;
        paddle2_position.y = 0;
        paddle1_dir = 0;
        paddle2_dir = 0;
        ball_speed = 1 + abs(score) * 0.1; //consecutive wins raise the difficulty
        wintext_timer = 1000;
    }

    paddle1_modelMatrix = glm::translate(glm::mat4(1.0f), paddle1_position);
    paddle1_modelMatrix = glm::scale(paddle1_modelMatrix, paddle1_scale);
    paddle2_modelMatrix = glm::translate(glm::mat4(1.0f), paddle2_position);
    paddle2_modelMatrix = glm::scale(paddle2_modelMatrix, paddle2_scale);
    ball_modelMatrix = glm::translate(glm::mat4(1.0f), ball_position);
    ball_modelMatrix = glm::rotate(ball_modelMatrix, time_total*3, vec_out);
    ball_modelMatrix = glm::scale(ball_modelMatrix, ball_scale);
    wintext_modelMatrix = glm::translate(glm::mat4(1.0f), wintext_position);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddle2_vertices);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, paddle2_uvs);
    glEnableVertexAttribArray(program.positionAttribute);
    glEnableVertexAttribArray(program.texCoordAttribute);
    program.SetModelMatrix(paddle2_modelMatrix);
    glBindTexture(GL_TEXTURE_2D, paddle2_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddle1_vertices);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, paddle1_uvs);
    glEnableVertexAttribArray(program.positionAttribute);
    glEnableVertexAttribArray(program.texCoordAttribute);
    program.SetModelMatrix(paddle1_modelMatrix);
    glBindTexture(GL_TEXTURE_2D, paddle1_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ball_vertices);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, ball_uvs);
    glEnableVertexAttribArray(program.positionAttribute);
    glEnableVertexAttribArray(program.texCoordAttribute);
    program.SetModelMatrix(ball_modelMatrix);
    glBindTexture(GL_TEXTURE_2D, ball_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, wintext_vertices);
    if (whowon == -1) {
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, wintext_uvs_1);
    } else if (whowon == 1) {
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, wintext_uvs_2);
    }
    glEnableVertexAttribArray(program.positionAttribute);
    glEnableVertexAttribArray(program.texCoordAttribute);
    program.SetModelMatrix(wintext_modelMatrix);
    glBindTexture(GL_TEXTURE_2D, wintext_texture);
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