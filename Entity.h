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

class Entity {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 scale;
    glm::vec3 acceleration;
    float rotation;

    GLuint textureID;

    glm::mat4 modelMatrix;

    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 1;
    int animRows = 1;

    Entity();

    void Update(float deltaTime);
    void Render(ShaderProgram* program);
    void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);
};