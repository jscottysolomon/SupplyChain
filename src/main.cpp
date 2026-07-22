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

#include <vector>
#include <iostream>
#include <csignal>
#include <cstdlib>

#include <raylib.h>
#include <string>

#include "road.hpp"
#include "traffic_dispatch.hpp"
#include "util.hpp"

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
void ui_handler();

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
    // InitWindow(1920/2, 1080/2, "SupplyChain");
    InitWindow(1920, 1080, "SupplyChain");
    SetTargetFPS(TICK_RATE);

    TrafficDispatch traffic;
    //TODO: move traffic control here

    SetGlobalTime();
    rlImGuiSetup(true);

    while (!WindowShouldClose())
    {
        if(IsWindowResized()) {

        }
        traffic.OnTick();

        BeginDrawing();
            ClearBackground(background);
            traffic.Draw();

            rlImGuiBegin();

            // show ImGui Content
            // bool open = true;
            // ImGui::ShowDemoWindow(&open);
            

            // open = true;
            // if (ImGui::Begin("Test Window", &open))
            // {
            //     ImGui::TextUnformatted(ICON_FA_JEDI);

            // }
            // ImGui::End();

            ui_handler();

            // end ImGui Content
            rlImGuiEnd();
        EndDrawing();
        
    }

    rlImGuiShutdown();

    CloseWindow();

    return 0;
}

void segfault_handler(int signal) {
    exit(EXIT_FAILURE); 
}

void ui_handler() {
    Vector2 vec = GetMousePosition();

    ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    // Main body of the Demo window starts here.
    ImGui::Begin("Hello, world!"); 
    ImGui::Text("This is some useful text.");   
    ImGui::SeparatorText("2-wide");
    static float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
        static int vec4i[4] = { 1, 5, 100, 255 };

        static ImGuiSliderFlags flags = 0;
        ImGui::InputFloat2("input float2", &screen_pos.x);
        ImGui::InputFloat2("input float2", &screen_pos.y);
        ImGui::DragFloat2("drag float2", &vec.x, 0.01f, 0.0f, 1.0f, NULL, flags);
        ImGui::DragFloat2("drag float2", &vec.y, 1, 0, 255, NULL, flags);
        ImGui::SliderFloat2("slider float2", vec4f, 0.0f, 1.0f, NULL, flags);
        ImGui::SliderInt2("slider int2", vec4i, 0, 255, NULL, flags);
    ImGui::End();
}