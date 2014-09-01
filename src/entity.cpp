/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements the base class for all game entities.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include <stdio.h>
#include "entity.hpp"
#include "bullet.hpp"
#include "player.hpp"
#include "input.hpp"
#include "display.hpp"

/*/////////////////
//   Constants   //
/////////////////*/
/// @summary The global EntityManager instance.
EntityManager* EntityManager::EM = NULL;

/*///////////////////////
//   Local Functions   //
///////////////////////*/

/*///////////////////////
//  Public Functions   //
///////////////////////*/
Entity::Entity(void) :
    Image(NULL),
    Orientation(0.0f),
    Radius(0.0f),
    IsExpired(false),
    Kind(ENTITY_DONT_CARE)
{
    Color[0] = 1.0f;
    Color[1] = 1.0f;
    Color[2] = 1.0f;
    Color[3] = 1.0f;
}

Entity::~Entity(void)
{
    /* empty */
}

void Entity::Input(double currentTime, double elapsedTime, InputManager *im)
{
    UNUSED_ARG(currentTime);
    UNUSED_ARG(elapsedTime);
    UNUSED_ARG(im);
}

void Entity::Draw(double currentTime, double elapsedTime, DisplayManager *dm)
{
    UNUSED_ARG(currentTime);
    UNUSED_ARG(elapsedTime);
    float  width   = (float) Image->GetWidth();
    float  height  = (float) Image->GetHeight();
    rect_t src     = { 0, 0, width, height };
    float  posx    = Position[0];
    float  posy    = Position[1];
    float  originx = width  * 0.5f;
    float  originy = height * 0.5f;
    float  scalex  = 1.0f;
    float  scaley  = 1.0f;
    dm->GetBatch()->Add(1, Image, posx, posy, src, Color, Orientation, originx, originy, scalex, scaley);
}

EntityManager* EntityManager::GetInstance(void)
{
    return EM;
}

EntityManager::EntityManager(void)
    :
    IsUpdating(false)
{
    EntityManager::EM = this;
}

EntityManager::~EntityManager(void)
{
    for (std::list<Entity*>::iterator i = Entities.begin(); i != Entities.end(); ++i)
    {
        delete *i;
        *i = NULL;
    }
    Entities.clear();
    AddedEntities.clear();
    Bullets.clear();
    Players.clear();
}

size_t EntityManager::PlayerCount(void) const
{
    return Players.size();
}

size_t EntityManager::EntityCount(void) const
{
    return Entities.size();
}

Player* EntityManager::GetPlayer(int index)
{
    std::list<Player*>::iterator iter = Players.begin();
    while (iter != Players.end())
    {
        if ((*iter)->GetIndex() == index)
            return *iter;
        ++iter;
    }
    return NULL;
}

void EntityManager::Add(Entity *entity)
{
    if (IsUpdating == false)
    {
        AddEntity(entity);
    }
    else AddedEntities.push_back(entity);
}

void EntityManager::AddEntity(Entity *entity)
{
    Entities.push_back(entity);
    switch (entity->GetKind())
    {
        case ENTITY_BULLET:
            Bullets.push_back((Bullet*) entity);
            break;

        case ENTITY_ENEMY:
            break;

        case ENTITY_BLACKHOLE:
            break;

        case ENTITY_PLAYER:
            Players.push_back((Player*) entity);
            break;

        default:
            break;
    }
}

void EntityManager::Update(double currentTime, double elapsedTime)
{
    printf("EntityManager: Begin update\n");
    IsUpdating = true;
    for (std::list<Entity*>::iterator i = Entities.begin(); i != Entities.end(); ++i)
    {
        (*i)->Update(currentTime, elapsedTime);
        if ((*i)->GetExpired())
            *i = NULL;
    }
    IsUpdating = false;

    // add entities created during the update:
    for (std::list<Entity*>::iterator i = AddedEntities.begin(); i != AddedEntities.end(); ++i)
    {
        AddEntity(*i);
    }
    AddedEntities.clear();
    Entities.remove(NULL);

    // prune bullet objects.
    for (std::list<Bullet*>::iterator i = Bullets.begin(); i != Bullets.end(); ++i)
    {
        if ((*i)->GetExpired())
        {
            delete *i;
            *i = NULL;
        }
    }
    Bullets.remove(NULL);
    printf("EntityManager: End update\n");
}

void EntityManager::Input(double currentTime, double elapsedTime, InputManager *im)
{
    printf("EntityManager: Begin input\n");
    for (std::list<Entity*>::iterator i = Entities.begin(); i != Entities.end(); ++i)
    {
        (*i)->Input(currentTime, elapsedTime, im);
    }
    printf("EntityManager: End input\n");
}

void EntityManager::Draw(double currentTime, double elapsedTime, DisplayManager *dm)
{
    printf("EntityManager: Begin draw\n");
    for (std::list<Entity*>::iterator i = Entities.begin(); i != Entities.end(); ++i)
    {
        (*i)->Draw(currentTime, elapsedTime, dm);
    }
    printf("EntityManager: End draw\n");
}
