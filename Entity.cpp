/**
* Author: JumboDS64 / Julien Olsson
* Assignment: Rise of the AI
* Date due: 2023-07-21, xx:xxpm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Entity.h"
#include "Map.h"

Sprite::Sprite(float x, float y) {
    position = glm::vec3(0);
    position.x = x;
    position.y = y;
    modelMatrix = glm::mat4(1.0f);
}
void Sprite::Render(ShaderProgram* program) {
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);

    program->SetModelMatrix(modelMatrix);

    float u = (float)(animIndex % animCols) / (float)animCols;
    float v = (float)(animIndex / animCols) / (float)animRows;

    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;

    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v };

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
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
}

void Entity::Update(float deltaTime, Map *map) {
    position += velocity * deltaTime;
    check_collision(map);
    sprite->position = position;
    sprite->position.y += 0.1;
}

void Entity::Render(ShaderProgram* program) {
    sprite->Render(program);
}

void Entity::check_collision(Map* map) {
    glm::vec3 temp = glm::vec3(position.x, position.y - colbox_height, 0); //bottom
    collided_bottom = false;
    if (map->is_solid(temp) && velocity.y < 0) {
        position.y = map->round_to_corner_y(position.y + colbox_height) - 0.1;
        velocity.y = 0;
        collided_bottom = true;
    }
    temp.y = position.y + colbox_height; //top
    collided_top = false;
    if (map->is_solid(temp) && velocity.y > 0) {
        position.y = map->round_to_corner_y(position.y - colbox_height);
        velocity.y = 0;
        collided_top = true;
    }
    temp.y = position.y;
    temp.x = position.x - colbox_width; //left
    collided_left = false;
    if (map->is_solid(temp) && velocity.x < 0) {
        position.x = map->round_to_corner_x(position.x + colbox_width);
        velocity.x = 0;
        collided_left = true;
    }
    temp.x = position.x + colbox_width; //right
    collided_right = false;
    if (map->is_solid(temp) && velocity.x > 0) {
        position.x = map->round_to_corner_x(position.x - colbox_width);
        velocity.x = 0;
        collided_right = true;
    }
}

Entity_Player::Entity_Player(float x, float y, GLuint tex) : Entity(x, y) {
    sprite->scale = glm::vec3(0.5);
    sprite->textureID = tex;
    sprite->animCols = 4;
    sprite->animRows = 4;
    colbox_height = 0.2;
    colbox_width = 0.2;
}
void Entity_Player::check_collision(Entity* ent) {
    if (position.x + colbox_width < ent->position.x - ent->colbox_width || position.x - colbox_width > ent->position.x + ent->colbox_width) return;
    if (position.y + colbox_height >= ent->position.y - ent->colbox_height && position.y - colbox_height <= ent->position.y + ent->colbox_height) {
        if (!ent->ded) {
            if (position.y > ent->position.y) {
                velocity.y = 2.5;
                ent->ded = true;
                win_state += 1;
            }
            else {
                win_state = -1;
            }
        }
    }
}

Entity_Enemy1::Entity_Enemy1(float x, float y, GLuint tex) : Entity(x, y) {
    sprite->scale = glm::vec3(0.5);
    sprite->textureID = tex;
    sprite->animCols = 4;
    sprite->animRows = 4;
    sprite->animIndex = 0;
    colbox_height = 0.2;
    colbox_width = 0.2;
    face = -1;
}
void Entity_Enemy1::Update(float deltaTime, Map* map) {
    if (!collided_bottom) {
        velocity.y -= 4 * deltaTime;
    }
    if (face == 1) {
        velocity.x = 1;
        if (collided_right) {
            face = -1;
        }
    }
    else if (face == -1) {
        velocity.x = -1;
        if (collided_left) {
            face = 1;
        }
    }
    Entity::Update(deltaTime, map);
}
Entity_Enemy2::Entity_Enemy2(float x, float y, GLuint tex) : Entity(x, y) {
    sprite->scale = glm::vec3(0.5);
    sprite->textureID = tex;
    sprite->animCols = 4;
    sprite->animRows = 4;
    sprite->animIndex = 1;
    colbox_height = 0.2;
    colbox_width = 0.2;
}
void Entity_Enemy2::Update(float deltaTime, Map* map) {
    timer += deltaTime;
    velocity.x = cos(timer * 2);
    velocity.y = sin(timer * 2);
    Entity::Update(deltaTime, map);
}
Entity_Enemy3::Entity_Enemy3(float x, float y, GLuint tex) : Entity(x, y) {
    sprite->scale = glm::vec3(0.5);
    sprite->textureID = tex;
    sprite->animCols = 4;
    sprite->animRows = 4;
    sprite->animIndex = 2;
    colbox_height = 0.2;
    colbox_width = 0.2;
}
void Entity_Enemy3::Update(float deltaTime, Map* map, glm::vec3 playerPos) {
    if (position.x < playerPos.x) {
        velocity.x += 2*deltaTime;
    } else if (position.x > playerPos.x) {
        velocity.x -= 2* deltaTime;
    }
    if (!collided_bottom) {
        velocity.y -= 4 * deltaTime;
    } else if (collided_left || collided_right) {
        velocity.y = 3;
    }
    Entity::Update(deltaTime, map);
}