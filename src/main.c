/* main.c - program entry point: opens the window and runs the game loop. */
#include "raylib.h"
#include "game.h"

int main(void)
{

    
    InitWindow(SCREEN_W, SCREEN_H, "The Verdict - Case File 05: The Last Piece");
    ChangeDirectory(GetApplicationDirectory());  /* find assets/ when installed */

    InitAudioDevice();
    Music bgm = LoadMusicStream("assets/music/bgm.mp3");
    SetMusicVolume(bgm, 0.30f);
    PlayMusicStream(bgm);
    
    SetExitKey(KEY_NULL);                 /* ESC does not close the game   */
    SetTargetFPS(60);
   

    GameState game;
    initializeGame(&game);

    while (!WindowShouldClose() && !game.shouldQuit) {
        UpdateMusicStream(bgm);
        BeginDrawing();
        ClearBackground((Color){ 20, 24, 32, 255 });
        updateAndDraw(&game);
        EndDrawing();
    }
    StopMusicStream(bgm);
    UnloadMusicStream(bgm);
    CloseAudioDevice();

    CloseWindow();
    return 0;
}