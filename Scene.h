#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Map.h"

class Entity;

class GameState {
public:
    float time_total;
    bool action_left = false;
    bool action_right = false;
    bool action_up = false;
    bool action_down = false;
    Map* map;
    Entity* entities[64];
    int entities_cur = 0;
    int entities_max = 64;
    bool flag_redActive = false;
    bool flag_greenActive = false;
    bool levelWon = false;
    int currentLevel = 1;
    bool addEntity(Entity* e);
    bool destroyEntity(int id);
};
class Scene
{
public:
    virtual void init();
    virtual void act(float deltaTime);
    virtual void draw(ShaderProgram* program);
};
class Scene_Title : public Scene
{
public:
    Entity* spr_title;
    ~Scene_Title();
    void init();
    void act(float deltaTime);
    void draw(ShaderProgram* program);
};
class Scene_Level : public Scene
{
public:
    glm::vec3 cameraPos;
    GameState gameState;
    ~Scene_Level();
    void init();
    void act(float deltaTime);
    void draw(ShaderProgram* program);

    void loadLevel(unsigned int* LEVEL_DATA, unsigned int LEVEL_ENTS_SIZE, unsigned int* LEVEL_ENTS);
    void loadLevel(int id);
    void clearEntities();
    void doCollision(Entity* e1, Entity* e2);
};