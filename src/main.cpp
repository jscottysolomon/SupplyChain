/**
 * @file main.cpp
 * 
 * @brief Main entry point, which handles the main game loops
 * and calls modules to perform their specific operations.
 * 
 * @author J. Scotty Solomon
 * @date 6-May-24
*/

#include "main.hpp"

#include <stdlib.h>

#include <vector>
#include <iostream>
#include <csignal>
#include <cstdlib>

#include <raylib.h>
#include <nlohmann/json.hpp>

#define TICK_RATE 60

float resize_factor = 1.0;
float resized_tile_size = TILE_SIZE * resize_factor;
// float zoom = 2.00;
float zoom = 3.75;
//full scree zoom is 7.5

/**
 * @brief Unloads all loaded textures and allocated memory.
 */
void clean_up();


/**
 * @brief Attempts to clean-up allocated memory after a
 * segfault occurs but before the program exits.
 * @param signal 
 */
void segfault_handler(int signal);

/*Temp variables. Values will be loaded from save file in future*/


Color background = {56,24,35,225};

/**
 * @brief Main game loop. Is in charge of managing all modules and 
 * interactions between modules.
 * @return int
 */
int main(void)
{
    signal(SIGSEGV, segfault_handler);

    /*Graphics*/
    // InitWindow(256*zoom, 144*zoom, "Game");
    InitWindow(1920/2, 1080/2, "Game");
    SetTargetFPS(TICK_RATE);
    // ToggleFullscreen();
    // setup_hud(inventory);

    while (!WindowShouldClose())
    {
        if(IsWindowResized()) {

        }
        BeginDrawing();
            DrawText("Hello, World!",0,0,20,WHITE);
        EndDrawing();
        
    }

    // clean_up();
    CloseWindow();

    return 0;
}

void segfault_handler(int signal) {
    // clean_up();
    exit(EXIT_FAILURE); 
}