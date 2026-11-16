/* ============================================================================
 *  case_data.c - THE STORY. Case File 05: The Last Piece.
 *  To change the case, edit only this file (and CULPRIT_INDEX in game.h).
 *
 *  Suspect order : 0 Eleanor, 1 Richard, 2 Victoria, 3 Thomas, 4 Dr. Adrian Cross
 *  Clue order    : 0 Burned Letter, 1 Pocket Watch, 2 USB Drive,
 *                  3 Brass Key, 4 Notebook Page, 5 Fabric Fiber
 * ========================================================================= */
#include <stdio.h>
#include <string.h>
#include "game.h"

static void setClue(Evidence *e, const char *name, const char *obs,
                    const char *analysis, int pointsTo)
{
    snprintf(e->name, sizeof e->name, "%s", name);
    snprintf(e->description, sizeof e->description, "%s", obs);
    snprintf(e->analysis, sizeof e->analysis, "%s", analysis);
    e->pointsTo = pointsTo;
    e->collected = 0;
    e->analyzed = 0;
}

static void setSuspect(Suspect *s, const char *name, const char *role, const char *alibi,
                       const char *q1, const char *a1,
                       const char *q2, const char *a2,
                       const char *q3, const char *a3)
{
    const char *q[QUESTIONS_PER] = { q1, q2, q3 };
    const char *a[QUESTIONS_PER] = { a1, a2, a3 };

    snprintf(s->name, sizeof s->name, "%s", name);
    snprintf(s->role, sizeof s->role, "%s", role);
    snprintf(s->alibi, sizeof s->alibi, "%s", alibi);
    for (int i = 0; i < QUESTIONS_PER; i++) {
        snprintf(s->questions[i], sizeof s->questions[i], "%s", q[i]);
        snprintf(s->answers[i], sizeof s->answers[i], "%s", a[i]);
        s->asked[i] = 0;
    }
    s->questioned = 0;
}

void loadCaseData(GameState *g)
{
    /* ---------------- EVIDENCE (observation + hidden lab result) ---------------- */
    setClue(&g->clues[0], "The Burned Letter",
        "A partially burned letter addressed to Alexander lies in the fireplace grate. One line is "
        "still legible: \"You will regret changing the will.\" The handwriting resembles Eleanor's.",
        "LAB RESULT: Eleanor did write it. The unburned edge mentions the divorce settlement and her "
        "share of the inheritance. It is a bitter legal threat about money, not a threat to his life. "
        "It proves a quarrel, not a murder.", 0);

    setClue(&g->clues[1], "The Broken Pocket Watch",
        "Found beside the victim's desk. The hands are stopped at 9:27 PM. It is engraved with the "
        "initials R.B.",
        "LAB RESULT: The watch belongs to Richard. Its spring cracked when he dropped it during a loud "
        "argument with Alexander earlier that evening. The stopped hands mark the moment of the "
        "argument, not the time of death.", 1);

    setClue(&g->clues[2], "The Black USB Drive",
        "Found beneath the bookshelf. It holds recordings of Alexander discussing secret financial "
        "transactions. One deleted audio file is partially recoverable.",
        "LAB RESULT: The drive came from a hidden recorder planted by Victoria, who was blackmailing "
        "Alexander over those transactions. The recovered audio is a fragment: Alexander warns someone "
        "that the ownership papers are forged and that he will expose it to the board at midnight. "
        "The name of the person he is speaking to is cut off.", 2);

    setClue(&g->clues[3], "The Brass Key",
        "Found near the study door. It opens the study's secondary entrance. Security records show "
        "that this key had gone missing from the key cabinet.",
        "LAB RESULT: Thomas had access to the cabinet, but the log shows the key was taken at 8:10 PM, "
        "when many people were in the house. It was wiped clean. Someone placed it near the door "
        "afterward to frame him. It shows a security failure, not a murder.", 3);

    setClue(&g->clues[4], "The Torn Notebook Page",
        "Contains notes on forged historical ownership documents. A handwritten sentence reads: "
        "\"He knows everything.\" The page was torn from a research notebook.",
        "LAB RESULT: The handwriting is Dr. Adrian Cross's and the torn edge matches his research "
        "notebook. The notes are genuine proof that he forged ownership papers, and that Alexander "
        "was about to expose him. This is a strong motive.", 4);

    setClue(&g->clues[5], "The Dark Fabric Fiber",
        "A small dark fiber is caught on the edge of the victim's desk. It looks like the fabric of "
        "someone's clothing. Under the microscope it carries an unusual restoration-wax residue.",
        "LAB RESULT: The fiber matches Dr. Cross's archival work coat, and the wax matches the "
        "restoration wax from his workshop. On its own this is NOT conclusive. Combined with the "
        "notebook page, the recovered recording and a broken alibi, it places him at the desk.", 4);

    /* ---------------- SUSPECTS ---------------- */
    setSuspect(&g->suspects[0], "Eleanor", "Alexander's estranged wife",
        "I was in the drawing room writing letters from 9:00 PM until 10:00 PM. I did not leave until "
        "I heard the commotion.",
        "Where were you at 9:27 PM?",
        "In the drawing room, alone. I know that is not much of an alibi, but I never went near the study.",
        "What was your relationship with Alexander?",
        "We were divorcing. He changed his will last week without telling me. I was angry, yes, but I am "
        "not a murderer.",
        "Is there evidence in the study that could point to you?",
        "The letter, probably. I wrote it and burned it. It was about the divorce settlement and my "
        "inheritance, nothing more.");

    setSuspect(&g->suspects[1], "Richard", "Alexander's business partner",
        "After arguing with Alexander in his study around 9:25 PM, I went out to the garden terrace to "
        "cool off and stayed there until 10:00 PM.",
        "Where were you at 9:27 PM?",
        "In the study, shouting at him. He was alive and shouting right back when I stormed out. Then I "
        "went to the terrace.",
        "What was your relationship with Alexander?",
        "Partners for twenty years. Lately he was obsessed with the ownership papers of a certain "
        "collection, and with one person's reports.",
        "Is there evidence in the study that could point to you?",
        "My pocket watch, I suppose. I must have dropped it during the argument. I did not even notice "
        "it was gone.");

    setSuspect(&g->suspects[2], "Victoria", "Alexander's financial secretary",
        "I was in the second-floor accounts office from 9:00 PM until 9:45 PM, then I went to my room.",
        "Where were you at 9:27 PM?",
        "In my office. At about 9:25 I walked to the printer and saw someone in a long dark coat near "
        "the study door. I did not see a face.",
        "What was your relationship with Alexander?",
        "I managed his private accounts. It was strictly professional.",
        "Is there evidence in the study that could point to you?",
        "No. Nothing of mine belongs in that room.");

    setSuspect(&g->suspects[3], "Thomas", "Head of estate security",
        "I was at the gatehouse monitoring the cameras from 9:00 PM until 10:00 PM. I only left when "
        "the alarm was raised.",
        "Where were you at 9:27 PM?",
        "At the gatehouse desk. My door log shows Dr. Cross badged out of the archive wing at 9:05 PM "
        "and back in at 9:55 PM.",
        "What was your relationship with Alexander?",
        "I have guarded this house for eleven years. Mr. Whitmore trusted me, and I would never betray "
        "that.",
        "Is there evidence in the study that could point to you?",
        "The study key, maybe. It vanished from my cabinet around eight o'clock. I did not take it and "
        "I did not leave it there. Someone is setting me up.");

    setSuspect(&g->suspects[4], "Dr. Adrian Cross", "Archivist and restorer",
        "I was in the archive wing cataloguing documents from 9:00 PM until 10:00 PM. I did not leave my "
        "workbench once.",
        "Where were you at 9:27 PM?",
        "At my workbench in the archive wing, with the restoration lamps on. I never left the room.",
        "What was your relationship with Alexander?",
        "Alexander was my patron and my friend. He funded my restoration work for years.",
        "Is there evidence in the study that could point to you?",
        "No. I have not set foot in the study in weeks. Anything found there has nothing to do with me.");
}

void initializeGame(GameState *g)
{
    memset(g, 0, sizeof *g);
    g->screen = SCREEN_MENU;
    g->selected = -1;
    g->selectedQ = -1;
    g->labSel = -1;
    g->accused = -1;
    loadCaseData(g);
}