/**
* Author: JumboDS64 / Julien Olsson
* Assignment: Rise of the AI
* Date due: 2023-07-21, xx:xxpm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Utility.h"
#include "Entity.h"
#include "Map.h"

Sprite::Sprite(float x, float y) {
    position = glm::vec3(0);
    position.x = x;
    position.y = y;
    modelMatrix = glm::mat4(1.0f);
}
void Sprite::load_texture(const char* filepath) {
    textureID = Utility::loadTexture(filepath);
}
void Sprite::Render(ShaderProgram* program) {
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, scale);

    program->SetModelMatrix(modelMatrix);

    float u = (float)(animIndex % animCols) / (float)animCols;
    float v = (float)(animIndex / animCols) / (float)animRows;

    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;

    float texCoords_[12] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    for (int i = 0; i < 12; i += 2) {
        texCoords_[i] = u + texCoords[i] * width;
        texCoords_[i+1] = v + texCoords[i+1] * height;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords_);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

Entity::Entity(float x, float y) {
    position = glm::vec3(0);
    position.x = x;
    position.y = y;
    sprite = new Sprite(x,y);
    velocity = glm::vec3(0);
    id = -1;
}

void Entity::act(float deltaTime, Map *map, GameState* gameState) {
    if (collided_top && velocity.y < 0) velocity.y = 0;
    if (collided_bottom && velocity.y > 0) velocity.y = 0;
    if (collided_left && velocity.x < 0) velocity.x = 0;
    if (collided_right && velocity.x > 0) velocity.x = 0;
    position += velocity * deltaTime;
    sprite->position = position;
    sprite->position.y += sprite_offsetY;
}

void Entity::render(ShaderProgram* program) {
    sprite->Render(program);
}
void Entity::doCollision(Map* map) {
}
void Entity::resolveCollision(float x, float y, float left, float right, float top, float bottom) {
    if (isAnchored) return;
    //assumes that there IS an overlap
    float overlap_left = (x + right) - (position.x - colbox_left); //on this entity's left side
    float overlap_right = (position.x + colbox_right) - (x - left); //ditto
    float overlap_top = (y + bottom) - (position.y - colbox_top);
    float overlap_bottom = (position.y + colbox_bottom) - (y - top);
    float overlap_width = overlap_left;
    if (overlap_left > overlap_right) overlap_width = overlap_right;
    float overlap_height = overlap_top;
    if (overlap_top > overlap_bottom) overlap_height = overlap_bottom;
    float x_avg = x + (right - left) / 2.0;
    float y_avg = y + (bottom - top) / 2.0;
    if (overlap_width > overlap_height) {
        if (position.y <= y_avg) {
            position.y = y - top - colbox_bottom;
            collided_bottom = true;
        }
        else if (position.y > y_avg) {
            position.y = y + bottom + colbox_top;
            collided_top = true;
        }
    }
    else {
        if (position.x <= x_avg) {
            position.x = x - left - colbox_right;
            collided_right = true;
        }
        else if (position.x > x_avg) {
            position.x = x + right + colbox_left;
            collided_left = true;
        }
    }
}
void Entity::resolveCollision(Entity* other) {
    if (!isSolid || !other->isSolid) return;
    resolveCollision(other->position.x, other->position.y, other->colbox_left, other->colbox_right, other->colbox_top, other->colbox_bottom);
}
void Entity::reactCollision(Entity* other) {
}
bool Entity::isPointInColbox(float x, float y) {
    return (position.x - colbox_left < x && x < position.x + colbox_right && position.y - colbox_top < y && y < position.y + colbox_bottom);
}

Entity_Player::Entity_Player(float x, float y) : Entity(x, y) {
    sprite->load_texture("player.png");
    sprite->animCols = 8;
    sprite->animRows = 3;
    sprite->scale = glm::vec3(3.125);
    sprite_offsetY = -3.125 / 2.0;
    colbox_top = 2.375;
    colbox_bottom = 0;
    colbox_left = 0.4375;
    colbox_right = 0.4375;
    face = 1;
    sfx_jump = Mix_LoadWAV("jump.wav");
    Mix_VolumeChunk(sfx_jump, MIX_MAX_VOLUME / 2);
    sfx_kick = Mix_LoadWAV("kick.wav");
    Mix_VolumeChunk(sfx_kick, MIX_MAX_VOLUME / 2);
}
Entity_Player::~Entity_Player() {
    Mix_FreeChunk(sfx_jump);
}

void Entity_Player::doCollision(Map* map) {
    //uses Classic Sonic method for now https://info.sonicretro.org/SPG:Solid_Tiles
    float ul;
    float dr;
    float closest;
    ul = map->getRightAt(position.x - colbox_left - 0.01, position.y - colbox_top + 0.1, +1);
    dr = map->getRightAt(position.x - colbox_left - 0.01, position.y + colbox_bottom - 0.1, -1);
    closest = ul;
    if (dr > closest) closest = dr;
    if (position.x - colbox_left - 0.01 < closest) {
        position.x = closest + colbox_left + 0.01;
        collided_left = true;
    }
    else collided_left = false;
    ul = map->getLeftAt(position.x + colbox_right + 0.01, position.y - colbox_top + 0.1, +1);
    dr = map->getLeftAt(position.x + colbox_right + 0.01, position.y + colbox_bottom - 0.1, -1);
    closest = ul;
    if (dr < closest) closest = dr;
    if (position.x + colbox_right + 0.01 > closest) {
        position.x = closest - colbox_right - 0.01;
        collided_right = true;
    }
    else collided_right = false;
    ul = map->getTopAt(position.x - colbox_left, position.y + colbox_bottom, +1);
    dr = map->getTopAt(position.x + colbox_right, position.y + colbox_bottom, -1);
    closest = ul;
    if (dr < closest) closest = dr;
    if (position.y + colbox_bottom > closest) {
        position.y = closest - colbox_bottom;
        collided_bottom = true;
    }
    else collided_bottom = false;
    ul = map->getBottomAt(position.x - colbox_left, position.y - colbox_top, +1);
    dr = map->getBottomAt(position.x + colbox_right, position.y - colbox_top, -1);
    closest = ul;
    if (dr > closest) closest = dr;
    if (position.y - colbox_top < closest) {
        position.y = closest + colbox_top;
        collided_top = true;
    }
    else collided_top = false;
}
void Entity_Player::act(float deltaTime, Map* map, GameState* gameState) {
    if (gameState->action_left) {
        sprite->animIndex = 8 + floor(fmod(gameState->time_total * 14.0, 8.0));
        velocity.x = -6;
        face = -1;
        sprite->scale.x = -3.125;
    }
    else if (gameState->action_right) {
        sprite->animIndex = 8 + floor(fmod(gameState->time_total * 14.0, 8.0));
        velocity.x = 6;
        face = 1;
        sprite->scale.x = 3.125;
    }
    else {
        sprite->animIndex = 0;
        velocity.x = 0;
    }
    if (collided_bottom && gameState->action_up) {
        velocity.y = -20;
        Mix_PlayChannel(0, sfx_jump, 0);
    }
    if (gameState->action_down && kickTimer <= 0) {
        kickTimer = 0.5;
        Mix_PlayChannel(0, sfx_kick, 0);
        if (gameState->entities[1]->isPointInColbox(position.x + 1.1 * face, position.y - 0.5)) {
            if (((Entity_Crate*)(gameState->entities[1]))->isSliding == 0) {
                ((Entity_Crate*)(gameState->entities[1]))->isSliding = face;
            }
        }
    }
    if (kickTimer > 0) {
        if (kickTimer > 0.4) sprite->animIndex = 16;
        else sprite->animIndex = 17;
        kickTimer -= deltaTime;
    }
    //std::cout << "test\n";
    if (!collided_bottom) velocity.y += 48 * deltaTime;
    Entity::act(deltaTime, map, gameState);
}

Entity_Crate::Entity_Crate(float x, float y) : Entity(x, y) {
    sprite->load_texture("crate.png");
    sprite->animCols = 1;
    sprite->animRows = 1;
    sprite->scale = glm::vec3(2.0);
    sprite_offsetY = 0.0;
    colbox_top = 1.0;
    colbox_bottom = 1.0;
    colbox_left = 1.0;
    colbox_right = 1.0;
    isSliding = 0;
}
void Entity_Crate::act(float deltaTime, Map* map, GameState* gameState) {
    if (isSliding == 1) {
        if (collided_right) {
            isSliding = 0;
            velocity.x = 0;
            collided_right = false;
        } else velocity.x = 15;
    }
    else if (isSliding == -1) {
        if (collided_left) {
            isSliding = 0;
            velocity.x = 0;
            collided_left = false;
        } else velocity.x = -15;
    }
    else {
        velocity.x = 0;
        if (!collided_bottom) velocity.y += 48 * deltaTime;
    }
    if (velocity.x == 0
        && velocity.y == 0
        && (map->getTileTypeAt(position.x - 0.25, position.y + 0.25, +1, -1) == 7
            || map->getTileTypeAt(position.x + 0.25, position.y + 0.25, -1, -1) == 7)) {
        gameState->levelWon = true;
    }
    Entity::act(deltaTime, map, gameState);
}
void Entity_Crate::doCollision(Map* map) {
    float ul;
    float dr;
    float closest;
    if (isSliding == 1) {
        ul = map->getLeftAt(position.x + colbox_right, position.y - 0.3, +1);
        dr = map->getLeftAt(position.x + colbox_right, position.y + 0.3, -1);
        closest = ul;
        if (dr < closest) closest = dr;
        if (position.x + colbox_right > closest) {
            position.x = closest - colbox_right - 0.01;
            collided_right = true;
        }
        else collided_right = false;
    }
    else if (isSliding == -1) {
        ul = map->getRightAt(position.x - colbox_left, position.y - 0.3, +1);
        dr = map->getRightAt(position.x - colbox_left, position.y + 0.3, -1);
        closest = ul;
        if (dr > closest) closest = dr;
        if (position.x - colbox_left < closest) {
            position.x = closest + colbox_left + 0.01;
            collided_left = true;
        }
        else collided_left = false;
    }

    ul = map->getTopAt(position.x - colbox_left, position.y + colbox_bottom, +1);
    dr = map->getTopAt(position.x + colbox_right, position.y + colbox_bottom, -1);
    closest = ul;
    if (dr < closest) closest = dr;
    if (position.y + colbox_bottom > closest) {
        position.y = closest - colbox_bottom;
        collided_bottom = true;
    }
    else collided_bottom = false;
    ul = map->getBottomAt(position.x - colbox_left, position.y - colbox_top, +1);
    dr = map->getBottomAt(position.x + colbox_right, position.y - colbox_top, -1);
    closest = ul;
    if (dr > closest) closest = dr;
    if (position.y - colbox_top < closest) {
        position.y = closest + colbox_top;
        collided_top = true;
    }
    else collided_top = false;
}

Entity_Button::Entity_Button(float x, float y, int _color) : Entity(x, y) {
    color = _color;
    sprite->load_texture("button.png");
    sprite->animCols = 4;
    sprite->animRows = 3;
    sprite->animIndex = 4*color;
    sprite->scale = glm::vec3(2.0);
    sprite_offsetY = 0.0;
    colbox_top = 1.0;
    colbox_bottom = 0.0;
    colbox_left = 1.0;
    colbox_right = 1.0;
    isSolid = false;
    isAnchored = true;
}
void Entity_Button::act(float deltaTime, Map* map, GameState* gameState) {
    Entity::act(deltaTime, map, gameState);
    if (!isDown_p && isDown) {
        switch (color) {
        case 0:
            gameState->flag_redActive = !gameState->flag_redActive;
            break;
        case 1:
            gameState->flag_greenActive = !gameState->flag_greenActive;
            break;
        }
    }
    isDown_p = isDown;
    isDown = false;
}
void Entity_Button::reactCollision(Entity* other) {
    isDown = true;
}

Entity_Gate::Entity_Gate(float x_, float y_, int _color, int _dir) : Entity(x_ + 0.5, y_ + 0.5) {
    float x = x_ + 0.5;
    float y = y_ + 0.5;
    color = _color;
    sprite->load_texture("gate1.png");
    sprite->animCols = 3;
    sprite->animRows = 3;
    sprite->animIndex = color*3;
    sprite->scale = glm::vec3(1.0);
    sprite_offsetY = 0.0;
    sprite_shaft = new Sprite(x, y);
    sprite_shaft->load_texture("gate2.png"); //for the animation of it opening ill have to fiddle with the UVs of the sprite's model
    sprite_shaft->animCols = 1;
    sprite_shaft->animRows = 3;
    sprite_shaft->animIndex = color;
    sprite_shaft->scale = glm::vec3(1.0);
    sprite_cap = new Sprite(x, y);
    sprite_cap->textureID = sprite->textureID;
    sprite_cap->animCols = 3;
    sprite_cap->animRows = 3;
    sprite_cap->animIndex = 2 + color * 3;
    sprite_cap->scale = glm::vec3(1.0);
    colbox_top = 0.5;
    colbox_bottom = 0.5;
    colbox_left = 0.5;
    colbox_right = 0.5;
    isAnchored = true;
    dir = _dir;
    switch (dir) {
    case 0:
        sprite->rotation = 0;
        sprite_shaft->rotation = 0;
        sprite_cap->rotation = 0;
        break;
    case 1:
        sprite->rotation = 1.57079632679;
        sprite_shaft->rotation = 1.57079632679;
        sprite_cap->rotation = 1.57079632679;
        break;
    case 2:
        sprite->rotation = 3.14159265359;
        sprite_shaft->rotation = 3.14159265359;
        sprite_cap->rotation = 3.14159265359;
        break;
    case 3:
        sprite->rotation = 4.71238898038;
        sprite_shaft->rotation = 4.71238898038;
        sprite_cap->rotation = 4.71238898038;
        break;
    }
}
void Entity_Gate::act(float deltaTime, Map* map, GameState* gameState) {
    Entity::act(deltaTime, map, gameState);
    bool canExtend = true;
    switch (dir) {
    case 0:
        if (collided_right) canExtend = false;
        colbox_right = 0.5 + shaftLen;
        sprite_cap->position.x = position.x + shaftLen;
        break;
    case 1:
        if (collided_bottom) canExtend = false;
        colbox_bottom = 0.5 + shaftLen;
        sprite_cap->position.y = position.y + shaftLen;
        break;
    case 2:
        if (collided_left) canExtend = false;
        colbox_left = 0.5 + shaftLen;
        sprite_cap->position.x = position.x - shaftLen;
        break;
    case 3:
        if (collided_top) canExtend = false;
        colbox_top = 0.5 + shaftLen;
        sprite_cap->position.y = position.y - shaftLen;
        break;
    }
    switch (color) {
    case 0:
        if (gameState->flag_redActive) {
            if (canExtend) shaftLen += deltaTime * 12.0;
            //std::cout << "1";
        }
        else {
            shaftLen -= deltaTime * 12.0;
            if (shaftLen < 0) shaftLen = 0;
            //std::cout << "0";
        }
        break;
    case 1:
        if (gameState->flag_greenActive) {
            if (canExtend) shaftLen += deltaTime * 12.0;
        }
        else {
            shaftLen -= deltaTime * 12.0;
            if (shaftLen < 0) shaftLen = 0;
        }
        break;
    }
}
void Entity_Gate::render(ShaderProgram* program) {
    sprite_shaft->vertices[2] = 0.5 + shaftLen;
    sprite_shaft->vertices[4] = 0.5 + shaftLen;
    sprite_shaft->vertices[8] = 0.5 + shaftLen;
    sprite_shaft->texCoords[2] = 1.0 + shaftLen;
    sprite_shaft->texCoords[4] = 1.0 + shaftLen;
    sprite_shaft->texCoords[8] = 1.0 + shaftLen;
    sprite_shaft->Render(program);
    sprite_cap->Render(program);
    sprite->Render(program);
}
void Entity_Gate::doCollision(Map* map) {
    switch (dir) {
    case 0:
        collided_right = false;
        if (map->getLeftAt(position.x + colbox_right, position.y, +1) <= position.x + colbox_right) {
            collided_right = true;
        }
        break;
    case 1:
        collided_bottom = false;
        if (map->getTopAt(position.x, position.y + colbox_bottom, +1) <= position.y + colbox_bottom) {
            collided_bottom = true;
        }
        break;
    case 2:
        collided_left = false;
        if (map->getRightAt(position.x - colbox_left, position.y, -1) <= position.x - colbox_left) {
            collided_left = true;
        }
        break;
    case 3:
        collided_top = false;
        if (map->getBottomAt(position.x, position.y - colbox_top, -1) <= position.x - colbox_top) {
            collided_top = true;
        }
        break;
    }

}
void Entity_Gate::reactCollision(Entity* other) {
    switch (dir) {
    case 0:
        if (position.y - colbox_top < other->position.y + other->colbox_bottom
            && other->position.y - other->colbox_top < position.y + colbox_bottom) {
            if (other->position.x - other->colbox_left <= position.x + colbox_right) {
                collided_right = true;
            }
        }
        break;
    case 1:
        if (position.x - colbox_left < other->position.x + other->colbox_right
            && other->position.x - other->colbox_left < position.x + colbox_right) {
            if (other->position.y - other->colbox_top <= position.y + colbox_bottom) {
                collided_bottom = true;
            }
        }
        break;
    case 2:
        if (position.y - colbox_top < other->position.y + other->colbox_bottom
            && other->position.y - other->colbox_top < position.y + colbox_bottom) {
            if (position.x - colbox_left <= other->position.x + other->colbox_right) {
                collided_left = true;
            }
        }
        break;
    case 3:
        if (position.x - colbox_left < other->position.x + other->colbox_right
            && other->position.x - other->colbox_left < position.x + colbox_right) {
            if (position.y - colbox_top <= other->position.y + other->colbox_bottom) {
                collided_top = true;
            }
        }
        break;
    }
}