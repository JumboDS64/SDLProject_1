/**
* Author: JumboDS64
* Assignment: Lunar Lander
* Date due: 2023-07-11, 11:54pm
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

#include "Entity.h"

/**struct GameState {
    Entity* player;
};
GameState state;**/

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix;
float time_c;
float time_p;
float delta;
float time_total;
const glm::vec3 vec_out = glm::vec3(0, 0, 1);

Entity* ent_player;

float dim_x = 4.75;
float dim_x2 = 4.25;
float dim_y = 3.5;
int score = 0;
int whowon = 0;
int paddle1_dir = 0;
int paddle2_dir = 0;
float paddle_width = 1;

bool key_left = false;
bool key_right = false;
bool key_up = false;
bool key_down = false;

int cs = 0; //0 = normal, 1 = lose, 2 = win
int cs_timer = 0;

int tiles_width = 20;
int tiles_height = 15;
int tile_render_size = 32;
int tiles[15][20] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 1, 1},
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 3, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0}
};
Entity* tile_drawer;
/**
* corners = 5, 3.75 (divide each dimension by 4)
* 
**/
Entity* screen_message;
Entity* fuel_bar;
float fuel = 30;
float fuel_max = 30;

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
    displayWindow = SDL_CreateWindow("Luner Landar", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    ent_player = new Entity();
    ent_player->position = glm::vec3(0);
    ent_player->position.x = 3.75;
    ent_player->position.y = 1;
    ent_player->scale = glm::vec3(0.5);
    ent_player->textureID = load_texture("saucer.png");
    ent_player->animCols = 4;
    ent_player->animRows = 4;

    tile_drawer = new Entity();
    tile_drawer->scale = glm::vec3(0.5);
    tile_drawer->textureID = load_texture("tiles.png");
    tile_drawer->animCols = 4;
    tile_drawer->animRows = 4;

    screen_message = new Entity();
    screen_message->scale = glm::vec3(4);
    screen_message->textureID = load_texture("wintext.png");
    screen_message->animCols = 2;
    screen_message->animRows = 1;
    screen_message->position.x = 100;
    screen_message->Update(1);

    fuel_bar = new Entity();
    fuel_bar->position.x = -4;
    fuel_bar->scale = glm::vec3(1);
    fuel_bar->scale.x = 0.5;
    fuel_bar->scale.y = 4;
    fuel_bar->textureID = load_texture("fuel.png");

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
        default:
            break;
        }
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    key_left = keys[SDL_SCANCODE_A];
    key_right = keys[SDL_SCANCODE_D];
    key_up = keys[SDL_SCANCODE_W];
    key_down = keys[SDL_SCANCODE_S];
}

int ent_to_tile_x(float x) {
    int out = (int)(x*2 + tiles_width/2.0);
    if (out >= tiles_width) return tiles_width - 1;
    else if (out < 0) return 0;
}
int ent_to_tile_y(float y) {
    int out = tiles_height-(int)(y*2 + tiles_height/2.0)-1;
    if (out >= tiles_height) return tiles_height - 1;
    else if (out < 0) return 0;
}
int tile_at(float x, float y) {
    return tiles[ent_to_tile_y(y)][ent_to_tile_x(x)];
}
int tile_at(Entity* e) {
    return tiles[ent_to_tile_y(e->position.y)][ent_to_tile_x(e->position.x)];
}

void Update() {
    time_c = (float)SDL_GetTicks() / 1000.;
    delta = time_c - time_p;
    time_p = time_c;
    time_total += delta;

    if (cs == 0) {
        if (key_left && fuel > 0) {
            ent_player->acceleration.x = -2;
            fuel -= 2 * delta;
        }
        else if (key_right && fuel > 0) {
            ent_player->acceleration.x = 2;
            fuel -= 2 * delta;
        }
        else {
            ent_player->acceleration.x = 0;
            float s = 1.5;
            if (ent_player->velocity.x < s * delta) {
                ent_player->velocity.x += s * delta;
                if (ent_player->velocity.x > 0) ent_player->velocity.x = 0;
            }
            if (ent_player->velocity.x > s * delta) {
                ent_player->velocity.x -= s * delta;
                if (ent_player->velocity.x < 0) ent_player->velocity.x = 0;
            }
        }
        if (key_up && fuel > 0) {
            ent_player->acceleration.y = 2;
            ent_player->animIndex = 1;
            fuel -= 2 * delta;
        }
        else {
            ent_player->acceleration.y = -1;
            ent_player->animIndex = 0;
        }
        if (ent_player->velocity.y < -10) {
            ent_player->velocity.y = -10;
        }
    }
    if (fuel < 0) fuel = 0;

    float px_old = ent_player->position.x;
    float py_old = ent_player->position.y;
    ent_player->Update(delta);

    int t = tile_at(ent_player->position.x, ent_player->position.y - 0.25);
    if (t != 0) {
        ent_player->position.x = px_old;
        ent_player->velocity.x = 0;
        ent_player->position.y = py_old;
        ent_player->velocity.y = 0;
        if (t == 1) {
            cs = 1;
            ent_player->acceleration.x = 0;
            ent_player->acceleration.y = -1;
            ent_player->animIndex = 2;
            screen_message->position.x = 0;
            screen_message->animIndex = 0;
            screen_message->Update(1);
            float s = 1.5;
            if (ent_player->velocity.x < s * delta) {
                ent_player->velocity.x += s * delta;
                if (ent_player->velocity.x > 0) ent_player->velocity.x = 0;
            }
            if (ent_player->velocity.x > s * delta) {
                ent_player->velocity.x -= s * delta;
                if (ent_player->velocity.x < 0) ent_player->velocity.x = 0;
            }
        } else if (t == 3) {
            cs = 2;
            ent_player->acceleration.x = 0;
            ent_player->acceleration.y = -1;
            ent_player->animIndex = 3;
            screen_message->position.x = 0;
            screen_message->animIndex = 1;
            screen_message->Update(1);
            float s = 1.5;
            if (ent_player->velocity.x < s * delta) {
                ent_player->velocity.x += s * delta;
                if (ent_player->velocity.x > 0) ent_player->velocity.x = 0;
            }
            if (ent_player->velocity.x > s * delta) {
                ent_player->velocity.x -= s * delta;
                if (ent_player->velocity.x < 0) ent_player->velocity.x = 0;
            }
        }
    }
    fuel_bar->scale.y = (fuel / fuel_max) * 4;
    fuel_bar->Update(delta);

    //if (ent_player->position.y < -dim_y) ent_player->position.y = -dim_y;
    //if (ent_player->position.y > dim_y) ent_player->position.y = dim_y;
    //if (ent_to_tile_y(py_old) != ent_to_tile_y(ent_player->position.y)) {
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int iy = 0; iy < tiles_height; iy++) {
        tile_drawer->position.y = -(iy - 7)*0.5;
        for (int ix = 0; ix < tiles_width; ix++) {
            tile_drawer->position.x = (ix - 9.5)*0.5;
            tile_drawer->animIndex = tiles[iy][ix];
            tile_drawer->Update(1); //delta not needed here since it doesnt actually "move"
            tile_drawer->Render(&program);
        }
    }
    ent_player->Render(&program);
    screen_message->Render(&program);
    fuel_bar->Render(&program);

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