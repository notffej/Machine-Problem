/* ui.c - reusable drawing helpers declared in ui.h. */
#include <stdio.h>
#include <string.h>

#include "game.h"

bool drawButton(Rectangle r, const char *text)
{
    Vector2 m = GetMousePosition();
    bool hover = CheckCollisionPointRec(m, r);
    Color bg = hover ? (Color){ 90, 110, 160, 255 } : (Color){ 55, 70, 105, 255 };

    DrawRectangleRec(r, bg);
    DrawRectangleLinesEx(r, 2, (Color){ 150, 170, 210, 255 });

    int fs = 20;
    int tw = MeasureText(text, fs);
    DrawText(text, (int)(r.x + (r.width - tw) / 2), (int)(r.y + (r.height - fs) / 2), fs, RAYWHITE);

    return hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

int drawTextWrapped(const char *text, int x, int y, int maxWidth, int fontSize, Color color)
{
    char line[512] = "";
    char word[128] = "";
    char test[700] = "";
    const char *p = text;
    int lineH = fontSize + 6;

    while (*p) {
        while (*p == ' ' || *p == '\t' || *p == '\n')
            p++;

        if (*p == '\0')
            break;

        int n = 0;
        while (*p && *p != ' ' && *p != '\t' && *p != '\n' && n < 127) {
            word[n++] = *p++;
        }
        word[n] = '\0';

        if (line[0] == '\0') {
            snprintf(line, sizeof line, "%s", word);
        } else {
            snprintf(test, sizeof test, "%s %s", line, word);
            if (MeasureText(test, fontSize) <= maxWidth) {
                snprintf(line, sizeof line, "%s", test);
            } else {
                DrawText(line, x, y, fontSize, color);
                y += lineH;
                snprintf(line, sizeof line, "%s", word);
            }
        }

        if (MeasureText(line, fontSize) > maxWidth && line[0] != '\0') {
            DrawText(line, x, y, fontSize, color);
            y += lineH;
            line[0] = '\0';
        }
    }

    if (line[0]) {
        DrawText(line, x, y, fontSize, color);
        y += lineH;
    }
    return y;
}

void drawHeader(const char *title)
{
    DrawRectangle(0, 0, SCREEN_W, 70, (Color){ 35, 42, 60, 255 });
    int tw = MeasureText(title, 36);
    DrawText(title, (SCREEN_W - tw) / 2, 17, 36, GOLD);
}