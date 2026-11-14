/* ============================================================================
 *  game.c - the brain of the game: one function per screen, plus the ending
 *  logic. main.c calls updateAndDraw() every frame, which picks the screen.
 * ========================================================================= */
#include <stdio.h>
#include <string.h>
#include "game.h"
 

#define TXT 20

/* ------------------------------------------------------------------ helpers */

int countClues(const GameState *g)
{
    int n = 0;
    for (int i = 0; i < TOTAL_CLUES; i++) n += g->clues[i].collected;
    return n;
}

int countAnalyzed(const GameState *g)
{
    int n = 0;
    for (int i = 0; i < TOTAL_CLUES; i++) n += g->clues[i].analyzed;
    return n;
}

static int allSuspectsQuestioned(const GameState *g)
{
    for (int i = 0; i < TOTAL_SUSPECTS; i++)
        if (g->suspects[i].questioned == 0) return 0;
    return 1;
}

/* Wrong suspect -> BAD. Right suspect -> GOOD only if every clue was found AND
 * analyzed and every suspect was questioned; otherwise PARTIAL. */
static EndingType evaluateEnding(const GameState *g)
{
    if (g->accused != CULPRIT_INDEX) return ENDING_BAD;
    if (countClues(g) == TOTAL_CLUES && countAnalyzed(g) == TOTAL_CLUES &&
        allSuspectsQuestioned(g))
        return ENDING_GOOD;
    return ENDING_PARTIAL;
}

/* ---------------------------------------------------------- simple screens */

static void screenMenu(GameState *g)
{
    drawHeader("THE VERDICT");
    DrawText("A DETECTIVE MYSTERY GAME", 340, 110, 24, LIGHTGRAY);
    DrawText("A suspicious death has occurred.", 340, 170, TXT, RAYWHITE);
    DrawText("The police need your help solving it.", 340, 196, TXT, RAYWHITE);

    if (drawButton(BTN(350, 260, 300, 50), "Start New Case")) {
        initializeGame(g);
        g->screen = SCREEN_BRIEFING;
    }
    if (drawButton(BTN(350, 325, 300, 50), "Instructions")) g->screen = SCREEN_INSTRUCTIONS;
    if (drawButton(BTN(350, 390, 300, 50), "Exit Game"))    g->shouldQuit = 1;
}

static void screenInstructions(GameState *g)
{
    drawHeader("INSTRUCTIONS");
    int y = 100;
    y = drawTextWrapped("You are the detective. Click an option to choose it.", 60, y, 880, TXT, RAYWHITE);
    y = drawTextWrapped("1. Investigate the crime scene and click each of the 6 objects to collect it.",
                        60, y + 10, 880, TXT, RAYWHITE);
    y = drawTextWrapped("2. Question every suspect. Compare their alibis with each other and with the evidence.",
                        60, y + 10, 880, TXT, RAYWHITE);
    y = drawTextWrapped("3. Once all 6 pieces of evidence are collected, the Laboratory unlocks. Analysis "
                        "reveals what each clue really means. Some clues are misleading.",
                        60, y + 10, 880, TXT, RAYWHITE);
    y = drawTextWrapped("4. Review the Detective Notebook to cross-reference clues, lab results and statements.",
                        60, y + 10, 880, TXT, RAYWHITE);
    y = drawTextWrapped("5. When you are ready, accuse a suspect. No single clue is enough: your ending "
                        "depends on who you accuse and how thoroughly you investigated.",
                        60, y + 10, 880, TXT, RAYWHITE);
    if (drawButton(BTN(350, 560, 300, 50), "Back")) g->screen = SCREEN_MENU;
}

static void screenBriefing(GameState *g)
{
    drawHeader("CASE BRIEFING");
    DrawText("CASE FILE 05: THE LAST PIECE", 60, 95, 24, GOLD);
    DrawText("VICTIM: Alexander Whitmore", 60, 135, TXT, RAYWHITE);
    DrawText("LOCATION: Whitmore Estate, private study", 60, 163, TXT, RAYWHITE);
    DrawText("TIME DISCOVERED: 10:00 PM", 60, 191, TXT, RAYWHITE);
    DrawText("DIFFICULTY: EXTREME", 60, 219, TXT, RED);

    int y = 265;
    y = drawTextWrapped("Alexander Whitmore, a wealthy collector of historical artifacts, was found dead "
                        "in his private study. Six pieces of physical evidence were left in the room.",
                        60, y, 880, TXT, RAYWHITE);
    y = drawTextWrapped("Five people were present at the estate that evening. Each one is hiding something, "
                        "but only one of them is a murderer.",
                        60, y + 10, 880, TXT, RAYWHITE);
    y = drawTextWrapped("No single clue will name the killer. Investigate the scene, question the suspects, "
                        "run laboratory analysis, and connect the evidence.",
                        60, y + 10, 880, TXT, RAYWHITE);
    DrawText("Good luck, Detective.", 60, y + 20, TXT, GOLD);

    if (drawButton(BTN(350, 560, 300, 50), "Continue")) g->screen = SCREEN_CASE_MENU;
}

static void screenCaseMenu(GameState *g)
{
    char buf[96];
    int unlocked = (countClues(g) == TOTAL_CLUES);

    drawHeader("CASE MENU");
    snprintf(buf, sizeof buf, "Clues collected: %d/%d      Lab analyses: %d/%d",
             countClues(g), TOTAL_CLUES, countAnalyzed(g), TOTAL_CLUES);
    DrawText(buf, 60, 90, TXT, LIGHTGRAY);

    if (drawButton(BTN(350, 130, 300, 50), "Investigate Crime Scene")) g->screen = SCREEN_CRIME_SCENE;
    if (drawButton(BTN(350, 195, 300, 50), "Question Suspects"))       g->screen = SCREEN_SUSPECTS;
    if (drawButton(BTN(350, 260, 300, 50), unlocked ? "Laboratory Analysis" : "Laboratory (locked)")
        && unlocked) {
        g->labSel = -1;
        g->screen = SCREEN_LAB;
    }
    if (drawButton(BTN(350, 325, 300, 50), "Detective Notebook")) {
        g->nbTab = 0; g->nbSel = 0; g->screen = SCREEN_NOTEBOOK;
    }
    if (drawButton(BTN(350, 390, 300, 50), "Accuse a Suspect"))        g->screen = SCREEN_ACCUSE;
    if (drawButton(BTN(350, 455, 300, 50), "Exit Game"))               g->shouldQuit = 1;

    if (!unlocked)
        DrawText("Collect all 6 pieces of evidence to unlock the laboratory.", 290, 525, 18, LIGHTGRAY);
}

/* ------------------------------------------------------------- crime scene */

static void screenCrimeScene(GameState *g)
{
    drawHeader("CRIME SCENE");
    DrawText("Click an object to inspect it.", 60, 85, TXT, LIGHTGRAY);

    for (int i = 0; i < TOTAL_CLUES; i++) {
        char label[80];
        Rectangle r = BTN(150 + (i % 2) * 380, 120 + (i / 2) * 125, 320, 100);
        snprintf(label, sizeof label, "%s%s", g->clues[i].name, g->clues[i].collected ? "  [found]" : "");
        if (drawButton(r, label)) {
            g->selected = i;
            g->newClue = !g->clues[i].collected;
            g->clues[i].collected = 1;
            g->screen = SCREEN_EVIDENCE;
        }
    }
    if (drawButton(BTN(350, 560, 300, 50), "Return to Case Menu")) g->screen = SCREEN_CASE_MENU;
}

static void screenEvidence(GameState *g)
{
    const Evidence *e = &g->clues[g->selected];
    drawHeader("EVIDENCE FOUND");
    DrawText(e->name, 60, 100, 28, GOLD);
    int y = drawTextWrapped(e->description, 60, 160, 880, 22, RAYWHITE);
    if (g->newClue) DrawText("[NEW CLUE ADDED TO NOTEBOOK]", 60, y + 20, TXT, GREEN);
    if (drawButton(BTN(350, 560, 300, 50), "Back to Crime Scene")) g->screen = SCREEN_CRIME_SCENE;
}

/* --------------------------------------------------------------- suspects */

static void screenSuspects(GameState *g)
{
    drawHeader("SUSPECT INTERVIEWS");
    for (int i = 0; i < TOTAL_SUSPECTS; i++) {
        char label[120];
        snprintf(label, sizeof label, "%s%s", g->suspects[i].name,
                 g->suspects[i].questioned ? "  (interviewed)" : "");
        if (drawButton(BTN(330, 100 + i * 80, 340, 48), label)) {
            g->selected = i;
            g->selectedQ = -1;
            g->screen = SCREEN_INTERVIEW;
        }
        DrawText(g->suspects[i].role, 345, 100 + i * 80 + 52, 16, LIGHTGRAY);
    }
    if (drawButton(BTN(350, 560, 300, 50), "Return to Case Menu")) g->screen = SCREEN_CASE_MENU;
}

static void screenInterview(GameState *g)
{
    Suspect *s = &g->suspects[g->selected];
    char buf[160];

    drawHeader("SUSPECT INTERVIEW");
    snprintf(buf, sizeof buf, "Suspect: %s  -  %s", s->name, s->role);
    DrawText(buf, 60, 85, 24, GOLD);
    DrawText("ALIBI:", 60, 128, TXT, LIGHTGRAY);
    int y = drawTextWrapped(s->alibi, 60, 154, 880, TXT, RAYWHITE);

    DrawText("Ask a question:", 60, y + 8, TXT, LIGHTGRAY);
    for (int q = 0; q < QUESTIONS_PER; q++) {
        if (drawButton(BTN(60, y + 36 + q * 46, 880, 40), s->questions[q])) {
            g->selectedQ = q;
            if (!s->asked[q]) { s->asked[q] = 1; s->questioned++; }
        }
    }
    if (g->selectedQ >= 0) {
        int py = y + 36 + QUESTIONS_PER * 46 + 4;
        DrawRectangle(60, py, 880, 572 - py, (Color){ 35, 42, 60, 255 });
        snprintf(buf, sizeof buf, "%s:", s->name);
        DrawText(buf, 75, py + 8, TXT, GOLD);
        drawTextWrapped(s->answers[g->selectedQ], 75, py + 36, 850, TXT, RAYWHITE);
    }
    if (drawButton(BTN(350, 582, 300, 48), "Back to Suspects")) g->screen = SCREEN_SUSPECTS;
}

/* ------------------------------------------------------------- laboratory */

static void screenLab(GameState *g)
{
    char buf[64];
    drawHeader("LABORATORY ANALYSIS");
    DrawText("Select an item to analyze it.", 40, 82, 18, LIGHTGRAY);

    for (int i = 0; i < TOTAL_CLUES; i++) {
        char label[80];
        snprintf(label, sizeof label, "%s%s", g->clues[i].name, g->clues[i].analyzed ? "  [done]" : "");
        if (drawButton(BTN(40, 110 + i * 58, 280, 50), label)) {
            g->labSel = i;
            g->clues[i].analyzed = 1;
        }
    }
    snprintf(buf, sizeof buf, "Analyzed: %d/%d", countAnalyzed(g), TOTAL_CLUES);
    DrawText(buf, 40, 470, TXT, GOLD);

    DrawRectangle(340, 110, 620, 440, (Color){ 35, 42, 60, 255 });
    if (g->labSel < 0) {
        DrawText("No item selected.", 360, 130, TXT, LIGHTGRAY);
    } else {
        const Evidence *e = &g->clues[g->labSel];
        DrawText(e->name, 360, 125, 26, GOLD);
        DrawText("OBSERVATION:", 360, 170, 18, LIGHTGRAY);
        int y = drawTextWrapped(e->description, 360, 196, 580, 18, RAYWHITE);
        drawTextWrapped(e->analysis, 360, y + 20, 580, 18, SKYBLUE);
    }
    if (drawButton(BTN(350, 580, 300, 50), "Return to Case Menu")) g->screen = SCREEN_CASE_MENU;
}

/* --------------------------------------------------------------- notebook */

static void screenNotebook(GameState *g)
{
    char buf[96];
    int n = (g->nbTab == 0) ? TOTAL_CLUES : TOTAL_SUSPECTS;

    drawHeader("DETECTIVE NOTEBOOK");
    if (drawButton(BTN(40, 82, 220, 40), "Collected Evidence")) { g->nbTab = 0; g->nbSel = 0; }
    if (drawButton(BTN(270, 82, 220, 40), "Suspect Statements")) { g->nbTab = 1; g->nbSel = 0; }

    for (int i = 0; i < n; i++) {
        const char *label;
        if (g->nbTab == 0) label = g->clues[i].collected ? g->clues[i].name : "???";
        else               label = g->suspects[i].name;
        if (drawButton(BTN(40, 140 + i * 56, 260, 46), label)) g->nbSel = i;
    }

    DrawRectangle(330, 140, 630, 430, (Color){ 35, 42, 60, 255 });
    if (g->nbTab == 0) {
        const Evidence *e = &g->clues[g->nbSel];
        if (e->collected) {
            DrawText(e->name, 350, 155, 26, GOLD);
            int y = drawTextWrapped(e->description, 350, 195, 590, 18, RAYWHITE);
            if (e->analyzed) drawTextWrapped(e->analysis, 350, y + 12, 590, 18, SKYBLUE);
            else             DrawText("Lab analysis pending.", 350, y + 12, 18, LIGHTGRAY);
        } else {
            DrawText("Not yet discovered.", 350, 155, TXT, LIGHTGRAY);
        }
        snprintf(buf, sizeof buf, "Clues collected: %d/%d     Analyzed: %d/%d",
                 countClues(g), TOTAL_CLUES, countAnalyzed(g), TOTAL_CLUES);
        DrawText(buf, 350, 540, 18, LIGHTGRAY);
    } else {
        const Suspect *s = &g->suspects[g->nbSel];
        if (s->questioned == 0) {
            snprintf(buf, sizeof buf, "You have not interviewed %s yet.", s->name);
            DrawText(buf, 350, 155, TXT, LIGHTGRAY);
        } else {
            int y = 155;
            snprintf(buf, sizeof buf, "SUSPECT: %s", s->name);
            DrawText(buf, 350, y, 24, GOLD);
            y = drawTextWrapped(s->alibi, 350, y + 32, 590, 16, RAYWHITE) + 6;
            for (int q = 0; q < QUESTIONS_PER; q++) {
                if (!s->asked[q]) continue;
                y = drawTextWrapped(s->questions[q], 350, y, 590, 16, SKYBLUE);
                y = drawTextWrapped(s->answers[q], 350, y, 590, 16, RAYWHITE) + 6;
            }
        }
    }
    if (drawButton(BTN(350, 585, 300, 45), "Return to Case Menu")) g->screen = SCREEN_CASE_MENU;
}

/* -------------------------------------------------------------- accusation */

static void screenAccuse(GameState *g)
{
    char buf[96];
    drawHeader("FINAL ACCUSATION");
    DrawText("You have reached the end of your investigation.", 60, 85, TXT, RAYWHITE);
    DrawText("Review your evidence carefully before proceeding.", 60, 111, TXT, RAYWHITE);
    snprintf(buf, sizeof buf, "Collected clues: %d/%d     Lab analyses: %d/%d",
             countClues(g), TOTAL_CLUES, countAnalyzed(g), TOTAL_CLUES);
    DrawText(buf, 60, 145, TXT, GOLD);
    DrawText("Who do you believe is responsible?", 60, 180, TXT, RAYWHITE);

    for (int i = 0; i < TOTAL_SUSPECTS; i++) {
        if (drawButton(BTN(330, 215 + i * 60, 340, 50), g->suspects[i].name)) {
            g->accused = i;
            g->endStage = 0;
            g->screen = SCREEN_ENDING;
        }
    }
    if (drawButton(BTN(325, 545, 350, 50), "Cancel and return to Case Menu")) g->screen = SCREEN_CASE_MENU;
}

/* ------------------------------------------------------------------ ending */

static void screenEnding(GameState *g)
{
    EndingType end = evaluateEnding(g);
    const Suspect *killer = &g->suspects[CULPRIT_INDEX];
    char buf[160];

    /* Stage 0: announce the accusation */
    if (g->endStage == 0) {
        drawHeader("FINAL ACCUSATION");
        snprintf(buf, sizeof buf, "You have accused %s.", g->suspects[g->accused].name);
        DrawText(buf, 300, 250, 28, GOLD);
        if (drawButton(BTN(350, 560, 300, 50), "Reveal the Verdict")) g->endStage = 1;
        return;
    }

    /* Stage 1: ending type and the killer's name */
    if (g->endStage == 1) {
        const char *title, *msg;
        if (end == ENDING_GOOD) {
            title = "GOOD ENDING";
            msg = "You identified the murderer and left no stone unturned. Every clue was collected and "
                  "analyzed, and every suspect was questioned.";
        } else if (end == ENDING_PARTIAL) {
            title = "PARTIAL ENDING";
            msg = "You identified the correct suspect. However, your investigation was incomplete. Some "
                  "evidence was never collected or analyzed, or some suspects were never questioned.";
        } else {
            title = "BAD ENDING";
            msg = "You accused the wrong person. The real murderer walks free.";
        }
        drawHeader(title);
        int y = drawTextWrapped(msg, 60, 100, 880, 22, RAYWHITE);
        snprintf(buf, sizeof buf, "THE KILLER: %s", killer->name);
        DrawText(buf, 60, y + 25, 28, RED);

        if (end == ENDING_BAD) {
            for (int i = 0; i < TOTAL_CLUES; i++) {
                if (g->clues[i].pointsTo == g->accused) {
                    snprintf(buf, sizeof buf, "WHY THE EVIDENCE AGAINST %s WAS MISLEADING:",
                             g->suspects[g->accused].name);
                    DrawText(buf, 60, y + 80, TXT, GOLD);
                    drawTextWrapped(g->clues[i].analysis, 60, y + 110, 880, TXT, RAYWHITE);
                    break;
                }
            }
        }
        if (drawButton(BTN(350, 560, 300, 50), "Next")) g->endStage = 2;
        return;
    }

    /* Stage 2: what really happened */
    if (g->endStage == 2) {
        drawHeader("CASE SOLUTION");
        DrawText("WHAT REALLY HAPPENED:", 60, 100, TXT, GOLD);
        int y = drawTextWrapped("Dr. Adrian Cross had forged the ownership documents of a collection that "
                                "Alexander Whitmore trusted him to authenticate. Alexander discovered the "
                                "forgery and planned to expose him to the board at midnight.",
                                60, 130, 880, TXT, RAYWHITE);
        y = drawTextWrapped("Around 9:30 PM, shortly after Richard's argument with Alexander, Adrian slipped "
                            "out of the archive wing, entered the study and killed him. His coat left a "
                            "fiber, carrying restoration wax, on the edge of the desk.",
                            60, y + 12, 880, TXT, RAYWHITE);
        y = drawTextWrapped("Someone then left the brass key near the study door to frame Thomas. The "
                            "evidence against the other four suspects was real, but it exposed only "
                            "unrelated secrets.", 60, y + 12, 880, TXT, RAYWHITE);
        if (drawButton(BTN(350, 560, 300, 50), "Next")) g->endStage = 3;
        return;
    }

    /* Stage 3: the false alibi and conclusion */
    drawHeader("CASE SOLUTION");
    DrawText("THE FALSE ALIBI:", 60, 100, TXT, GOLD);
    int y = drawTextWrapped("Adrian claimed he never left his workbench between 9:00 and 10:00 PM, and "
                            "that he had not set foot in the study in weeks. But Thomas's door log shows "
                            "he left the archive wing from 9:05 to 9:55 PM, Victoria saw a figure in a "
                            "long dark coat near the study door at about 9:25 PM, and a fiber from his "
                            "work coat was caught on Alexander's desk.",
                            60, 130, 880, TXT, RAYWHITE);
    DrawText("THE FINAL CONCLUSION:", 60, y + 20, TXT, GOLD);
    y = drawTextWrapped("The notebook page and the recovered recording gave him a motive. The fiber and "
                        "wax placed him at the desk. The door log and witness broke his alibi. Eleanor, "
                        "Richard, Victoria and Thomas were each hiding a different secret: an "
                        "inheritance dispute, an argument, blackmail and a security failure.",
                        60, y + 50, 880, TXT, RAYWHITE);
    DrawText("Dr. Adrian Cross was responsible for Alexander Whitmore's death.", 60, y + 20, TXT, RED);
    if (drawButton(BTN(350, 560, 300, 50), "Back to Main Menu")) initializeGame(g);
}

/* -------------------------------------------------------------- dispatcher */

void updateAndDraw(GameState *g)
{
    switch (g->screen) {
        case SCREEN_MENU:         screenMenu(g);         break;
        case SCREEN_INSTRUCTIONS: screenInstructions(g); break;
        case SCREEN_BRIEFING:     screenBriefing(g);     break;
        case SCREEN_CASE_MENU:    screenCaseMenu(g);     break;
        case SCREEN_CRIME_SCENE:  screenCrimeScene(g);   break;
        case SCREEN_EVIDENCE:     screenEvidence(g);     break;
        case SCREEN_SUSPECTS:     screenSuspects(g);     break;
        case SCREEN_INTERVIEW:    screenInterview(g);    break;
        case SCREEN_LAB:          screenLab(g);          break;
        case SCREEN_NOTEBOOK:     screenNotebook(g);     break;
        case SCREEN_ACCUSE:       screenAccuse(g);       break;
        case SCREEN_ENDING:       screenEnding(g);       break;
    }
}