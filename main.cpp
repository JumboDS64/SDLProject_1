/**
* Author: JumboDS64 / Julien Olsson
* Assignment: Cavrncrate (C++ Port)
* Date due: 2023-08-10, 1:00PM
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
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Utility.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix;
float time_c;
float time_p;
float delta;
const glm::vec3 vec_out = glm::vec3(0, 0, 1);
int startuptimer = 30; //to prevent bugs from deltaTime being extremely high during startup due to loading lag

Scene* scene;
Scene_Title scene_title;
Scene_Level scene_level;
bool action_enter;
bool action_restart;
bool action_restart_p;

bool paused = true;

float gravity = -1;
Mix_Music* bgm;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Cavrncrate", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 1024, 768);
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(0.0f, 32.0f, 24.0f, 0.0f, 0.0f, 1.0f);

    Mix_OpenAudio(
        48000,        // the frequency to playback audio at (in Hz)
        MIX_DEFAULT_FORMAT,  // audio format
        2,      // number of channels (1 is mono, 2 is stereo, etc).
        4096      // audio buffer size in sample FRAMES (total samples divided by channel count)
    );
    bgm = Mix_LoadMUS("bgm.mp3");

    scene_title = Scene_Title();
    scene_title.init();
    scene_level = Scene_Level();
    scene_level.init();
    scene = &scene_title;

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.0f, 0.05f, 0.1f, 1.0f);

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
            paused = true;
            gameIsRunning = false;
            break;
        default:
            break;
        }
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    scene_level.gameState.action_left = keys[SDL_SCANCODE_A];
    scene_level.gameState.action_right = keys[SDL_SCANCODE_D];
    scene_level.gameState.action_up = keys[SDL_SCANCODE_W];
    scene_level.gameState.action_down = keys[SDL_SCANCODE_S];
    action_restart_p = action_restart;
    action_restart = keys[SDL_SCANCODE_R];
    action_enter = keys[SDL_SCANCODE_RETURN];
}

void Update() {
    time_c = (float)SDL_GetTicks() / 1000.;
    delta = time_c - time_p;
    time_p = time_c;

    if (startuptimer > 0) {
        startuptimer--;
        if (startuptimer == 0) {
            paused = false;
        }
    }

    if (!paused) {
        if (scene == &scene_title && action_enter) {
            scene = &scene_level;
            scene_level.init();
            //Mix_PlayMusic(bgm, -1);
            //Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
        }
        if (scene == &scene_level && scene_level.gameState.currentLevel == 5) {
            scene = &scene_title;
            Mix_HaltMusic();
        }
        if (scene == &scene_level && !action_restart_p && action_restart) {
            scene_level.loadLevel(scene_level.gameState.currentLevel);
        }
        scene->act(delta);
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    viewMatrix = glm::mat4(1.0f);
    if (scene == &scene_level) {
        viewMatrix = glm::translate(viewMatrix, scene_level.cameraPos);
    }
    program.SetViewMatrix(viewMatrix);

    scene->draw(&program);
    

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    Mix_FreeMusic(bgm);
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