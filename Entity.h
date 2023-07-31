#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Map.h"

class Sprite {
public:
    glm::vec3 position;
    glm::vec3 scale;
    float rotation;
    GLuint textureID;
    glm::mat4 modelMatrix;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 1;
    int animRows = 1;
    Sprite(float x, float y);
    void Render(ShaderProgram* program);
};

class Entity {
public:
    Sprite* sprite;
    glm::vec3 position;
    glm::vec3 velocity;

    float colbox_width = 0.8f;
    float colbox_height = 0.8f;
    bool collided_top = false;
    bool collided_bottom = false;
    bool collided_left = false;
    bool collided_right = false;

    bool ded = false;

    Entity(float x, float y);

    void Update(float deltaTime, Map *map);
    void Render(ShaderProgram* program);
    void check_collision(Map* map);
};

class Entity_Player : public Entity {
public:
    bool player_grounded = true;
    int win_state = 0; //-1 = lose, 0 = normal, 1 = 1 enemy killed, 2 = 2 enemies killed, 3 = win
    Entity_Player(float x, float y, GLuint tex);
    void check_collision(Entity* ent);
};
class Entity_Enemy1 : public Entity {
private:
    int face;
public:
    Entity_Enemy1(float x, float y, GLuint tex);
    void Update(float deltaTime, Map* map);
};
class Entity_Enemy2 : public Entity {
private:
    float timer;
public:
    Entity_Enemy2(float x, float y, GLuint tex);
    void Update(float deltaTime, Map* map);
};
class Entity_Enemy3 : public Entity {
public:
    Entity_Enemy3(float x, float y, GLuint tex);
    void Update(float deltaTime, Map* map, glm::vec3 playerPos);
};