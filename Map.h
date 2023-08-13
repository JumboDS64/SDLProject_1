#pragma once
#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <vector>
#include <math.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

class Map {
private:
    int m_width;
    int m_height;

    // Here, the level_data is the numerical "drawing" of the map
    unsigned int* m_level_data;
    GLuint m_texture_id;

    float tileSize;
    int   m_tile_count_x;
    int   m_tile_count_y;

    // Just like with rendering text, we're rendering several sprites at once
    // So we need vectors to store their respective vertices and texture coordinates
    std::vector<float> m_vertices;
    std::vector<float> m_texture_coordinates;
    float getTopAt_(float x, float y, int bias);
    float getBottomAt_(float x, float y, int bias);
    float getLeftAt_(float x, float y, int bias);
    float getRightAt_(float x, float y, int bias);

public:
    // Constructor
    Map(unsigned int* level_data, GLuint texture_id, float tile_size, int
        tile_count_x, int tile_count_y);

    // Methods
    void build();
    void render(ShaderProgram* program);
    int getTileTypeAt(float x, float y, int bias_x, int bias_y);
    int getTileTypeAt_int(int tile_x, int tile_y);
    float getTopmap(int tile_type, float x);
    float getBottommap(int tile_type, float x);
    float getLeftmap(int tile_type, float y);
    float getRightmap(int tile_type, float y);
    float getTopAt(float x, float y, int bias);
    float getBottomAt(float x, float y, int bias);
    float getLeftAt(float x, float y, int bias);
    float getRightAt(float x, float y, int bias);
    float mathfloorToTile(float n);
    float mathceilToTile(float n);

    // Getters
    int const get_width()  const { return m_width; }
    int const get_height() const { return m_height; }

    unsigned int* const get_level_data() const { return m_level_data; }
    GLuint        const get_texture_id() const { return m_texture_id; }

    float const get_tile_size()    const { return tileSize; }
    int   const get_tile_count_x() const { return m_tile_count_x; }
    int   const get_tile_count_y() const { return m_tile_count_y; }

    std::vector<float> const get_vertices()            const { return m_vertices; }
    std::vector<float> const get_texture_coordinates() const { return m_texture_coordinates; }
};