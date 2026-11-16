/* ============================================================================
 *  FILE        : ui.h
 *  PROJECT     : THE VERDICT - A Detective Mystery Game
 *
 *  PURPOSE
 *  -------
 *  Declares the small reusable drawing helpers used by every screen:
 *      - a clickable button
 *      - a paragraph that wraps automatically
 *      - a title bar for the top of each screen
 *
 *  Putting these in one place means every screen has the same look, and the
 *  screen code in game.c stays short and readable. The code is written in
 *  ui.c.
 *
 *  INCLUDED BY : game.c, ui.c
 *  DEPENDS ON  : raylib.h (Rectangle, Color and drawing functions)
 * ========================================================================= */

#ifndef UI_H
#define UI_H

#include <stdbool.h>    /* for the bool type */
#include "raylib.h"


/* ----------------------------------------------------------------------------
 *  BTN(x, y, w, h)
 *  A shortcut that builds a raylib Rectangle.
 *      x, y : position of the top-left corner, in pixels
 *      w, h : width and height, in pixels
 *  Example:  drawButton(BTN(350, 260, 300, 50), "Start New Case")
 *            makes a button 300 x 50 pixels at position (350, 260).
 * ------------------------------------------------------------------------- */
#define BTN(x, y, w, h) ((Rectangle){ (x), (y), (w), (h) })


/* ----------------------------------------------------------------------------
 *  drawButton
 *  Draws a rectangular button with centered text and detects clicks.
 *    - The button is a darker blue normally and a lighter blue when the mouse
 *      is over it (hover effect).
 *    - The text is drawn at font size 20, centered inside the rectangle.
 *
 *  Parameters:
 *      r    : position and size of the button (use the BTN macro)
 *      text : the label shown on the button
 *
 *  Returns:
 *      true  on the frame the player releases the left mouse button while the
 *            cursor is on the button (a click)
 *      false otherwise
 *
 *  Usage:
 *      if (drawButton(BTN(350, 150, 300, 50), "Question Suspects")) {
 *          g->screen = SCREEN_SUSPECTS;
 *      }
 *  Because it draws AND checks the click in one call, no separate mouse
 *  handling code is needed.
 * ------------------------------------------------------------------------- */
bool drawButton(Rectangle r, const char *text);


/* ----------------------------------------------------------------------------
 *  drawTextWrapped
 *  Draws a long paragraph and automatically starts a new line whenever the
 *  next word would go past the allowed width. raylib's DrawText does not wrap
 *  text by itself, so this helper is used for all alibis, clue descriptions
 *  and story text.
 *
 *  Parameters:
 *      text     : the paragraph to draw (plain text; newline characters are
 *                 not supported, words are separated by spaces)
 *      x, y     : top-left position where the first line starts
 *      maxWidth : the widest a line may be, in pixels
 *      fontSize : text size in pixels. The gap between lines is
 *                 fontSize + 6 pixels.
 *      color    : text color (e.g. RAYWHITE, GOLD, LIGHTGRAY)
 *
 *  Returns:
 *      The y position just BELOW the last line drawn. Use it to place the next
 *      element underneath without guessing how many lines were used:
 *          int y = drawTextWrapped(alibi, 60, 158, 880, 20, RAYWHITE);
 *          DrawText("Ask a question:", 60, y + 15, 20, LIGHTGRAY);
 * ------------------------------------------------------------------------- */
int drawTextWrapped(const char *text, int x, int y, int maxWidth, int fontSize, Color color);


/* ----------------------------------------------------------------------------
 *  drawHeader
 *  Draws the title bar at the top of a screen: a dark band 70 pixels tall,
 *  full width, with the title centered in gold, font size 36.
 *
 *  Parameter:
 *      title : the screen name, e.g. "CASE MENU" or "DETECTIVE NOTEBOOK"
 *
 *  Note: content on every screen is placed below y = 70 so it does not
 *  overlap the header.
 * ------------------------------------------------------------------------- */
void drawHeader(const char *title);

#endif /* UI_H */