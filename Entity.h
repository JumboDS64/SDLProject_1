#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Map.h"
#include "Scene.h"

class Sprite {
public:
    glm::vec3 position;
    glm::vec3 scale;
    float rotation = 0;
    GLuint textureID;
    glm::mat4 modelMatrix;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 1;
    int animRows = 1;
    float vertices[12] = { -0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5 };
    float texCoords[12] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    Sprite(float x, float y);
    void load_texture(const char* filepath);
    void Render(ShaderProgram* program);
};

class Entity {
public:
    Sprite* sprite;
    float sprite_offsetY = 0.0;
    glm::vec3 position;
    glm::vec3 velocity;
    int id;

    float colbox_top = 0.5f;
    float colbox_bottom = 0.5f;
    float colbox_left = 0.5f;
    float colbox_right = 0.5f;
    bool collided_top = false;
    bool collided_bottom = false;
    bool collided_left = false;
    bool collided_right = false;
    bool isSolid = true;
    bool isAnchored = false;

    bool ded = false;

    Entity(float x, float y);

    virtual void act(float deltaTime, Map* map, GameState* gameState);
    virtual void render(ShaderProgram* program);
    virtual void doCollision(Map* map);
    void resolveCollision(float x, float y, float left, float right, float top, float bottom);
    virtual void resolveCollision(Entity* other);
    virtual void reactCollision(Entity* other);
    bool isPointInColbox(float x, float y);
};

class Entity_Player : public Entity {
private:
    int face;
    Mix_Chunk* sfx_jump; //giving each entity its own copy of the sfx is atrocious, but for now it works
    Mix_Chunk* sfx_kick;
    float kickTimer;
public:
    Entity_Player(float x, float y);
    ~Entity_Player();
    void doCollision(Map* map);
    void act(float deltaTime, Map* map, GameState* gameState);
};
class Entity_Crate : public Entity {
public:
    int isSliding;
    Entity_Crate(float x, float y);
    void act(float deltaTime, Map* map, GameState* gameState);
    void doCollision(Map* map);
};
class Entity_Button : public Entity {
public:
    int color;
    bool isDown_p = false;
    bool isDown = false;
    Entity_Button(float x, float y, int _color);
    void act(float deltaTime, Map* map, GameState* gameState);
    void reactCollision(Entity* other);
};
class Entity_Gate : public Entity {
private:
    int dir; //0 = right, 1 = down, 2 = left, 3 = up
    float shaftLen = 0;
    Sprite* sprite_shaft;
    Sprite* sprite_cap;
public:
    int color;
    Entity_Gate(float x, float y, int _color, int _dir);
    void act(float deltaTime, Map* map, GameState* gameState);
    void render(ShaderProgram* program);
    void doCollision(Map* map);
    void reactCollision(Entity* other);
};