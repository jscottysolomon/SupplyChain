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
#include <imgui/rlImGui.h>
#include <imgui/imgui.h>

#include <iostream>
#include <csignal>
#include <cstdlib>
#include <string>
#include <vector>

#include "road.hpp"
#include "traffic_mediator.hpp"
#include "util.hpp"
#include "ui_handler.hpp"

#define TICK_RATE 60

float resize_factor = 1.0;
float resized_tile_size = TILE_SIZE * resize_factor;
float zoom = 3.75;

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
void SetUpStyle();

Color background = {56,24,35,225};

// DPI scaling functions
float ScaleToDPIF(float value)
{
  return GetWindowScaleDPI().x * value;
}

int ScaleToDPII(int value)
{
  return int(GetWindowScaleDPI().x * value);
}

/**
 * @brief Main game loop. Is in charge of managing all modules and 
 * interactions between modules.
 * @return int
 */
int main(void)
{
  signal(SIGSEGV, segfault_handler);
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);

  /*Graphics*/
  InitWindow(1200, 800, "SupplyChain");

  // if(!IsWindowFullscreen()) ToggleFullscreen();
  SetTargetFPS(TICK_RATE);

  SetGlobalTime();
  TrafficMediator traffic;
  UiHandler uiHandler(traffic);

  rlImGuiSetup(true);

  #ifdef IMGUI_HAS_DOCK
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  #endif


  while (!WindowShouldClose())
  {
    if(IsWindowResized()) {

    }
    traffic.OnTick();

    BeginDrawing();
      ClearBackground(background);
      traffic.Draw();

      rlImGuiBegin();
        uiHandler.RenderUi();
      rlImGuiEnd();
      // DrawRectangle(GetMousePosition().x,GetMousePosition().y,5,5,WHITE);
    EndDrawing();
  }

  rlImGuiShutdown();
  CloseWindow();

  return 0;
}

void segfault_handler(int signal) {
  exit(EXIT_FAILURE); 
}
