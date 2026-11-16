/* ============================================================================
 *  FILE        : game.h
 *  PROJECT     : THE VERDICT - A Detective Mystery Game
 *  CASE        : CASE FILE 05 - THE LAST PIECE
 *  COURSE      : CMSC 11 - Fundamentals of Programming 1
 *                University of the Philippines Tacloban
 *
 *  PURPOSE
 *  -------
 *  The central header of the project. It defines everything the source files
 *  must agree on:
 *      1. Constants    (number of clues, suspects, window size, culprit)
 *      2. Data types   (Evidence, Suspect, GameScreen, EndingType, GameState)
 *      3. Prototypes   (functions shared between .c files)
 *
 *  It contains NO game logic. It only describes what the data looks like and
 *  which functions exist.
 *
 *  INCLUDED BY : main.c, game.c, case_data.c, ui.c
 *  DEPENDS ON  : raylib.h
 * ========================================================================= */

#ifndef GAME_H
#define GAME_H

#include "raylib.h"


/* ============================================================================
 *  SECTION 1: CONSTANTS
 * ========================================================================= */

/* Pieces of physical evidence at the crime scene (exactly six):
 *   0 Burned Letter, 1 Broken Pocket Watch, 2 Black USB Drive,
 *   3 Brass Key, 4 Torn Notebook Page, 5 Dark Fabric Fiber               */
#define TOTAL_CLUES        6

/* Number of suspects:
 *   0 Eleanor, 1 Richard, 2 Victoria, 3 Thomas, 4 Dr. Adrian Cross        */
#define TOTAL_SUSPECTS     5

/* Questions the player can ask each suspect.                              */
#define QUESTIONS_PER      3

/* Index of the real murderer in suspects[]. 4 = Dr. Adrian Cross.
 * The accusation is compared against this number to pick the ending.      */
#define CULPRIT_INDEX      4

/* Window size in pixels. All layouts in game.c are positioned using these. */
#define SCREEN_W           1000
#define SCREEN_H           650


/* ============================================================================
 *  SECTION 2: DATA STRUCTURES
 * ========================================================================= */

/* ----------------------------------------------------------------------------
 *  Evidence
 *  ONE object at the crime scene. The game has TWO layers of information:
 *    - OBSERVATION (description): what the player sees when inspecting it.
 *    - DEDUCTION   (analysis)   : what it really means. Hidden until the
 *                                 player runs laboratory analysis.
 * ------------------------------------------------------------------------- */
typedef struct {
    char name[50];          /* Title, e.g. "The Burned Letter".               */
    char description[500];  /* Observation layer, shown on inspection.        */
    char analysis[700];     /* Deduction layer (hidden truth / lab result),
                               shown only after laboratory analysis.          */
    int  pointsTo;          /* Index of the suspect this clue APPEARS to point
                               toward. Used to explain a wrong accusation.    */
    int  collected;         /* 0 = not found yet, 1 = found at the scene.     */
    int  analyzed;          /* 0 = lab analysis not run, 1 = analysis done.   */
} Evidence;

/* ----------------------------------------------------------------------------
 *  Suspect
 *  ONE person of interest with their alibi and answers.
 * ------------------------------------------------------------------------- */
typedef struct {
    char name[50];                       /* Display name, e.g. "Eleanor".     */
    char role[60];                       /* Relation to the victim.           */
    char alibi[500];                     /* Opening statement about where they
                                            were during the crime.            */
    char questions[QUESTIONS_PER][100];  /* Questions the player can ask.     */
    char answers[QUESTIONS_PER][500];    /* answers[i] replies to questions[i]*/
    int  asked[QUESTIONS_PER];           /* 1 once question i has been asked. */
    int  questioned;                     /* Count of questions asked (0-3).   */
} Suspect;

/* ----------------------------------------------------------------------------
 *  GameScreen
 *  Every screen of the game. Exactly one is active at a time (a "state
 *  machine"); clicking a button changes GameState.screen to move on.
 * ------------------------------------------------------------------------- */
typedef enum {
    SCREEN_MENU,          /* Main menu                                        */
    SCREEN_INSTRUCTIONS,  /* How to play                                      */
    SCREEN_BRIEFING,      /* Case number, victim, location, summary           */
    SCREEN_CASE_MENU,     /* Hub: scene, suspects, lab, notebook, accuse      */
    SCREEN_CRIME_SCENE,   /* Six clickable pieces of evidence                 */
    SCREEN_EVIDENCE,      /* Close-up of the evidence just clicked            */
    SCREEN_SUSPECTS,      /* List of the five suspects                        */
    SCREEN_INTERVIEW,     /* Alibi + three questions for one suspect          */
    SCREEN_LAB,           /* Laboratory analysis (unlocked after 6 clues)     */
    SCREEN_NOTEBOOK,      /* Review of evidence, lab results and statements   */
    SCREEN_ACCUSE,        /* Choose who to accuse                             */
    SCREEN_ENDING         /* Verdict and explanation (4 stages)               */
} GameScreen;

/* ----------------------------------------------------------------------------
 *  EndingType  (decided by evaluateEnding() in game.c)
 *    ENDING_BAD     : wrong suspect accused.
 *    ENDING_PARTIAL : right suspect, but investigation incomplete (not all
 *                     clues collected, not all analyzed, or a suspect was
 *                     never questioned).
 *    ENDING_GOOD    : right suspect, all 6 clues collected AND analyzed, and
 *                     every suspect questioned at least once.
 * ------------------------------------------------------------------------- */
typedef enum {
    ENDING_BAD,
    ENDING_PARTIAL,
    ENDING_GOOD
} EndingType;

/* ----------------------------------------------------------------------------
 *  GameState
 *  ALL data of the game in progress. One GameState is created in main() and
 *  passed by pointer (by reference) to every function, so no global
 *  variables are needed.
 * ------------------------------------------------------------------------- */
typedef struct {
    GameScreen screen;                   /* Current screen.                   */
    Evidence   clues[TOTAL_CLUES];       /* All evidence.                     */
    Suspect    suspects[TOTAL_SUSPECTS]; /* All suspects.                     */

    int selected;   /* Clue on SCREEN_EVIDENCE or suspect on SCREEN_INTERVIEW. */
    int selectedQ;  /* Question whose answer is shown (-1 = none yet).         */
    int newClue;    /* 1 = clue on screen was just discovered.                 */
    int labSel;     /* Clue selected in the laboratory (-1 = none).            */
    int nbTab;      /* Notebook tab: 0 evidence, 1 suspect statements.         */
    int nbSel;      /* Highlighted entry in the notebook list.                 */
    int accused;    /* Suspect accused (-1 = nobody yet).                      */
    int endStage;   /* 0 accusation, 1 verdict, 2 what happened, 3 false alibi */
    int shouldQuit; /* 1 = main loop should stop.                              */
} GameState;


/* ============================================================================
 *  SECTION 3: FUNCTION PROTOTYPES
 * ========================================================================= */

/* ----- case_data.c ------------------------------------------------------- */

/* initializeGame : resets a GameState to a new game (clears everything, sets
 *                  screen to SCREEN_MENU, selections to -1, then loads the
 *                  case). Called at startup and after every ending.        */
void initializeGame(GameState *g);

/* loadCaseData   : fills in the story: 6 clues (observation + lab result)
 *                  and 5 suspects (alibis, questions, answers).            */
void loadCaseData(GameState *g);

/* ----- game.c ------------------------------------------------------------ */

/* updateAndDraw  : runs one frame; draws the screen in g->screen and handles
 *                  clicks. Called about 60 times per second from main().    */
void updateAndDraw(GameState *g);

/* countClues     : how many clues are collected (0 to TOTAL_CLUES).        */
int countClues(const GameState *g);

/* countAnalyzed  : how many clues have been through the laboratory.        */
int countAnalyzed(const GameState *g);

#endif /* GAME_H */