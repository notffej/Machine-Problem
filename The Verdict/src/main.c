/* main.c - program entry point: opens the window and runs the game loop. */
#include "raylib.h"
#include "game.h"

int main(void)
{
    InitWindow(SCREEN_W, SCREEN_H, "The Verdict - Case File 05: The Last Piece");
    SetExitKey(KEY_NULL);                 /* ESC does not close the game   */
    SetTargetFPS(60);
    ChangeDirectory(GetApplicationDirectory());  /* find assets/ when installed */

    GameState game;
    initializeGame(&game);

    while (!WindowShouldClose() && !game.shouldQuit) {
        BeginDrawing();
        ClearBackground((Color){ 20, 24, 32, 255 });
        updateAndDraw(&game);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}