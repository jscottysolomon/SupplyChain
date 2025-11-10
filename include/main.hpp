/**
 * @file main.hpp
 * 
 * @brief File that stores variables/enums that require
 * global access
 * 
 * @author J. Scotty Solomon
 * @date 03-Apr-24
*/
#ifndef MAIN_HPP
#define MAIN_HPP

#define TILE_SIZE 16

#include <raylib.h>
#include <string>

/*png resize factor*/
extern float resize_factor;
extern float resized_tile_size;
extern float zoom;

// enum item_type

/*Entity direction*/
enum Direction {
    none = -1,
    down = 0,
    up = 2,
    right = 4,
    left = 6,
};

#define DIRECTION_FLIP 2

/**
 * @brief Struct to store room dimensions
 */
typedef struct Room {
    Rectangle rec;
    int id;     /*Room id*/
} Room;

/**
 * @brief Transformation of LayeredTile entity for rendering.
 */
typedef enum {
    None=0,
    flipV,
    flipH,
    r90,
    r270} 
Transformation;

enum Status {
    walk=0,
    attack,
    push,
    pull,
    carry,
    swim,
    underwater,
    shieldWalk,
    block
};

/*
    Environment
        Room
            Tiles
                LayeredTile
                    id
                    rotation
                    entity
            Rooms ("next room")
        Sheets
            SheetTile
            Source
        Tiles
            LayeredTile
                id
                rotation
                entity
        Enemies
        Collidable Entities
        Animated Tiles
    Player
        Inventory
            Equipment
                Sword
                Bow
                Et cetera
        Health
            Max Health
            Current Health
            Heart containers collected
    Input
        &Environment
        &Player
    */



#endif