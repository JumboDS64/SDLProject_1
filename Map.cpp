/**
* Author: JumboDS64 / Julien Olsson
* Assignment: Rise of the AI
* Date due: 2023-07-21, xx:xxpm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Map.h"

Map::Map(unsigned int* level_data, GLuint texture_id, float tile_size, int tile_count_x, int tile_count_y)
{
    m_width = 32;
    m_height = 24;

    m_level_data = level_data;
    m_texture_id = texture_id;

    tileSize = tile_size;
    m_tile_count_x = tile_count_x;
    m_tile_count_y = tile_count_y;

    build();
}

void Map::build()
{
    for (int y_coord = 0; y_coord < m_height; y_coord++)
    {
        for (int x_coord = 0; x_coord < m_width; x_coord++)
        {
            int tile = m_level_data[y_coord * m_width + x_coord];
            if (tile == 0) continue;

            float u_coord = (float)(tile % m_tile_count_x) / (float)m_tile_count_x;
            float v_coord = (float)(tile / m_tile_count_x) / (float)m_tile_count_y;

            float tile_width = 1.0f / (float)m_tile_count_x;
            float tile_height = 1.0f / (float)m_tile_count_y;

            m_vertices.insert(m_vertices.end(), {
                (tileSize * x_coord),  tileSize * y_coord,
                (tileSize * x_coord),  (tileSize * y_coord) + tileSize,
                (tileSize * x_coord) + tileSize, (tileSize * y_coord) + tileSize,
                (tileSize * x_coord), tileSize * y_coord,
                (tileSize * x_coord) + tileSize, (tileSize * y_coord) + tileSize,
                (tileSize * x_coord) + tileSize, tileSize * y_coord
                });

            m_texture_coordinates.insert(m_texture_coordinates.end(), {
                u_coord, v_coord,
                u_coord, v_coord + (tile_height),
                u_coord + tile_width, v_coord + (tile_height),
                u_coord, v_coord,
                u_coord + tile_width, v_coord + (tile_height),
                u_coord + tile_width, v_coord
                });
        }
    }
}

void Map::render(ShaderProgram* program)
{
    glm::mat4 model_matrix = glm::mat4(1.0f);
    program->SetModelMatrix(model_matrix);

    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, m_vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, m_texture_coordinates.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glDrawArrays(GL_TRIANGLES, 0, (int)m_vertices.size() / 2);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

int Map::getTileTypeAt(float x, float y, int bias_x, int bias_y) {
    int tile_x = floor(x / tileSize);
    if (floorf(x) == x && bias_x == -1) {
        tile_x--;
    }
    int tile_y = floor(y / tileSize);
    if (floorf(y) == y && bias_y == -1) {
        tile_y--;
    }
    return getTileTypeAt_int(tile_x, tile_y);
}
int Map::getTileTypeAt_int(int tile_x, int tile_y) {
    if (tile_x < 0 || tile_x >= m_width)  return 1;
    if (tile_y < 0 || tile_y >= m_height) return 1;
    // If the tile index is 0 i.e. an open space, it is not solid
    //int tile = m_level_data[tile_y * m_width + tile_x];
    return (m_level_data[tile_y * m_width + tile_x]);
}
float Map::getTopmap(int tile_type, float x) {
    // 0 = top of tile, 1 = bottom of tile
    switch (tile_type) {
    case 0: //nothing
    case 7: //goal tile
        return 1;
    case 1: //full tile
        return 0;
    case 2: //slope NW
        return 1 - fmod(x, 1); //1 on left, 0 on right
    case 3: //slope NE
        return fmod(x, 1); //0 on left, 1 on right
    case 4: //slope SW; flat on top
        return 0;
    case 5: //slope SE; flat on top
        return 0;
    case 6: //platform
        return 0;
    }
}
float Map::getBottommap(int tile_type, float x) {
    // 0 = bottom of tile, 1 = top of tile
    switch (tile_type) {
    case 0: //nothing
    case 7: //goal tile
        return 1;
    case 1: //full tile
        return 0;
    case 2: //slope NW; flat on bottom
        return 0;
    case 3: //slope NE; flat on bottom
        return 0;
    case 4: //slope SW
        return 1 - fmod(x, 1); //1 on left, 0 on right
    case 5: //slope SE
        return fmod(x, 1); //0 on left, 1 on right
    case 6: //platform; intangible on bottom
        return 1;
    }
}
float Map::getLeftmap(int tile_type, float y) {
    // 0 = leftside of tile, 1 = rightside of tile
    switch (tile_type) {
    case 0: //nothing
    case 7: //goal tile
        return 1;
    case 1: //full tile
        return 0;
    case 2: //slope NW
        return 1 - fmod(y, 1); //1 on top, 0 on bottom
    case 3: //slope NE; flat on leftside
        return 0;
    case 4: //slope SW
        return fmod(y, 1); //0 on top, 1 on bottom
    case 5: //slope SE; flat on leftside
        return 0;
    case 6: //platform; intangible on left
        return 1;
    }
}
float Map::getRightmap(int tile_type, float y) {
    // 0 = rightside of tile, 1 = leftside of tile
    switch (tile_type) {
    case 0: //nothing
    case 7: //goal tile
        return 1;
    case 1: //full tile
        return 0;
    case 2: //slope NW; flat on rightside
        return 0;
    case 3: //slope NE
        return 1 - fmod(y, 1); //1 on top, 0 on bottom
    case 4: //slope SW; flat on rightside
        return 0;
    case 5: //slope SE
        return fmod(y, 1); //0 on top, 1 on bottom
    case 6: //platform; intangible on right
        return 1;
    }
}
float Map::getTopAt(float x, float y, int bias) {
    float val = getTopmap(getTileTypeAt(x, y, bias, 1), x);
    if (val == 0) return getTopAt_(x, y - tileSize, bias);
    return mathfloorToTile(y) + val * tileSize;
}
float Map::getTopAt_(float x, float y, int bias) {
    float val = getTopmap(getTileTypeAt(x, y, bias, 1), x);
    return mathfloorToTile(y) + val * tileSize;
}

float Map::getBottomAt(float x, float y, int bias) {
    float val = getBottommap(getTileTypeAt(x, y, bias, 1), x);
    if (val == 0) return getBottomAt_(x, y + tileSize, bias);
    return mathceilToTile(y) - val * tileSize;
}
float Map::getBottomAt_(float x, float y, int bias) {
    float val = getBottommap(getTileTypeAt(x, y, bias, 1), x);
    return mathceilToTile(y) - val * tileSize;
}

float Map::getLeftAt(float x, float y, int bias) {
    float val = getLeftmap(getTileTypeAt(x, y, 1, bias), y);
    if (val == 0) return getLeftAt_(x - tileSize, y, bias);
    return mathfloorToTile(x) + val * tileSize;
}
float Map::getLeftAt_(float x, float y, int bias) {
    float val = getLeftmap(getTileTypeAt(x, y, 1, bias), y);
    return mathfloorToTile(x) + val * tileSize;
}

float Map::getRightAt(float x, float y, int bias) {
    float val = getRightmap(getTileTypeAt(x, y, 1, bias), y);
    if (val == 0) return getRightAt_(x + tileSize, y, bias);
    return mathceilToTile(x) - val * tileSize;
}
float Map::getRightAt_(float x, float y, int bias) {
    float val = getRightmap(getTileTypeAt(x, y, 1, bias), y);
    return mathceilToTile(x) - val * tileSize;
}

float Map::mathfloorToTile(float n) {
    return floor(n / tileSize) * tileSize;
}
float Map::mathceilToTile(float n) {
    return (floor(n / tileSize) + 1) * tileSize; //not actual ceil because otherwise integer coordinates would return identical results for floor() and ceil()
}