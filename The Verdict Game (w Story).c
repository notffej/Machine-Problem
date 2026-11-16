#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//GAME CONSTANTS
#define MAX_CASES 1
#define MAX_EVIDENCE 6
#define MAX_SUSPECT 5
#define MAX_QA 5

//DATA STRUCTURES
typedef struct {
    char name [50];
    char description [500];
    int collected;
} Evidence;

typedef struct {
    char name [50];
    char alibi [500];
    char questions [MAX_QA][100];
    char answers [MAX_QA][500];
    int asked[MAX_QA];
    int interviewed;
    int qaCount;
} Suspect;

typedef struct {
    char casename [50];
    char casebriefing[1000];
    Evidence evidence [MAX_EVIDENCE];
    Suspect suspect [MAX_SUSPECT];
    int evidenceCount;
    int suspectCount;
    char culprit [50];
    char solution [1000];
} Case;

//SCREEN MANAGER
void clearScreen();
void pauseScreen();

//INPUT CHECKER
int readNumber(int min, int max);

//MENU AND CASE SELECTION
void printMenu();
int caseSelection(const Case Files[]);

//INVESTIGATE EVIDENCE
void printEvidence(Case *currentCase);
void investigateEvidence(const Case *currentCase, int index);

//INTERROGATE SUSPECTS
void printSuspects(Case *currentCase);
void interrogateSuspect(Case *currentCase, int index);

//NOTEBOOK TO REVIEW PAST INFO
void reviewNotebook(const Case *currentCase);
void reviewEvidence(const Case *currentCase);
void reviewStatements (const Case *currentCase);

//ACCUSATION
int makeAccusation(const Case *currentCase);

//SOLUTION AND CONCLUSION
void revealSolution(const Case *currentCase, int index);

//REPOSITORY
void caseFiles(Case files[]);

int main (void) {
    int choice, selectedcase;

    Case files[MAX_CASES] = {0};
    caseFiles(files);

    printf("====================================\n");
    printf("             THE VERDICT            \n");
    printf("       A DETECTIVE MYSTERY GAME     \n");
    printf("====================================\n");

    printf("\nA suspicious death has occurred.\n");
    printf("The police need your help solving it.\n");

    printf("\n1. Start New Case\n");
    printf("2. Exit Game\n");
    //printf ("3. Credits\n");

    printf("\nChoose: ");
    choice = readNumber (1, 2);

    if (choice == 2) {
        clearScreen();
        printf("Goodbye, Detective.\n");
        return 0;
    }

    selectedcase = caseSelection(files);

    while (1) {
        printMenu();
        printf ("\nChoice: ");
        choice = readNumber(1, 5);

        switch (choice) {
            case 1:
                printEvidence(&files[selectedcase]);
                break;
            case 2:
                printSuspects(&files[selectedcase]);
                break;
            case 3:
                reviewNotebook(&files[selectedcase]);
                break;
            case 4:
                if (makeAccusation(&files[selectedcase])) {
                    return 0;
                }
                break;
            case 5:
                clearScreen();
                printf("Case abandoned. Goodbye, Detective.\n");
                return 0;
        }
    }

    return 0;
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pauseScreen() {
    int ch;

    printf("\nPress Enter to continue...");

    while ((ch = getchar()) != '\n' && ch != EOF);
}

int readNumber(int min, int max) {
    char input[100];
    char *end;
    long number;
    int ch;

    while (1) {
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nInput ended. Goodbye, Detective.\n");
            exit(0);
        }

        if (strchr(input, '\n') == NULL) {
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("Input too long. Try again: ");
            continue;
        }

        number = strtol(input, &end, 10);

        if (end == input) {
            printf("Invalid input. Enter a number: ");
            continue;
        }

        while (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r') {
            end++;
        }

        if (*end != '\0' || number < min || number > max) {
            printf("Invalid choice. Try again: ");
            continue;
        }

        return (int)number;
    }
}

int caseSelection(const Case files[]) {
    int choice;

    clearScreen();

    printf ("Cases Available:\n");

    for (int i = 0; i < MAX_CASES; i++) {
        printf ("   [%d]: %s\n", i+1, files[i].casename);
    }

    printf ("\nChoice: ");
    choice = readNumber (1, MAX_CASES);

    clearScreen();

    printf("====================================\n");
    printf("            CASE BRIEFING           \n");
    printf("====================================\n");
    printf ("%s", files[choice - 1].casebriefing);
    printf ("\nGood luck, Detective.\n");

    pauseScreen();

    return choice - 1;
}

void printMenu () {
    clearScreen();

    printf("=====================================\n");
    printf("             CASE MENU               \n");
    printf("=====================================\n");

    printf("    1. Investigate Crime Scene\n");
    printf("    2. Question Suspects\n");
    printf("    3. Review Detective Notebook\n");
    printf("    4. Accuse a Suspect\n");
    printf("    5. Exit Game\n");
}

void printEvidence(Case *currentCase) {
    int choice = 0;

    while (1) {
        clearScreen();

        printf("=====================================\n");
        printf("            CRIME SCENE               \n");
        printf("=====================================\n");

        printf("\nObjects available for inspection:\n");

        for (int i = 0; i < currentCase->evidenceCount; i++) {
            printf ("   %d. %s", i+1, currentCase->evidence[i].name);

            if (currentCase->evidence[i].collected == 1) {
                printf (" [COLLECTED]");
            }
            printf ("\n");
        }

        printf ("   %d. Return to Case Menu\n", currentCase->evidenceCount + 1);
        printf ("\nChoice: ");

        choice = readNumber (1, currentCase->evidenceCount + 1);

        if (choice == currentCase->evidenceCount + 1) {
            return;
        }

        currentCase->evidence[choice - 1].collected = 1;

        investigateEvidence(currentCase, choice - 1);
    }
}

void investigateEvidence(const Case *currentCase, int index) {
    clearScreen();

    printf("====================================\n");
    printf("          EVIDENCE FOUND            \n");
    printf("====================================\n");
    printf ("\nObject: %s\n\n", currentCase->evidence[index].name);
    printf ("Observation:\n%s\n", currentCase->evidence[index].description);

    pauseScreen();
}

void printSuspects (Case *currentCase) {
    int choice = 0, asked = 0;

    while (1) {
        clearScreen();

        printf("====================================\n");
        printf("            SUSPECT LIST            \n");
        printf("====================================\n");

        for (int i = 0; i < currentCase->suspectCount; i++) {

            printf("   %d. %s", i+1, currentCase->suspect[i].name);

            for (int j = 0; j < currentCase->suspect[i].qaCount; j++) {
                if (currentCase->suspect[i].asked[j] == 1) {
                    asked++;
                }
            }

            if (asked == currentCase->suspect[i].qaCount) {
                printf (" [INTERVIEWED]");
            }

            asked = 0;
            printf ("\n");
        }

        printf ("   %d. Return to Case Menu\n\n", currentCase->suspectCount + 1);
        printf ("Choice: ");
        choice = readNumber (1, currentCase->suspectCount +1);

        if (choice == currentCase->suspectCount + 1) {
            return;
        }

        interrogateSuspect(currentCase, choice - 1);
    }
}

void interrogateSuspect(Case *currentCase, int index) {
    int choice = 0;

    while (1) {
        clearScreen();
        printf("====================================\n");
        printf("         SUSPECT INTERVIEWS         \n");
        printf("====================================\n");

        printf ("Suspect: %s\n\n", currentCase->suspect[index].name);
        printf ("ALIBI:\n%s\n\n", currentCase->suspect[index].alibi);

        printf ("Questions:\n");

        for (int j = 0; j < currentCase->suspect[index].qaCount; j++) {
            printf ("   %d. %s", j+1, currentCase->suspect[index].questions[j]);

            if (currentCase->suspect[index].asked[j] == 1) {
                printf (" [QUESTIONED]");
            }
            printf ("\n");
        }

        printf ("\n   %d. Return to Suspect List\n", currentCase->suspect[index].qaCount + 1);
        printf ("\nChoice: ");
        choice = readNumber(1, currentCase->suspect[index].qaCount + 1);

        if (choice == currentCase->suspect[index].qaCount + 1) {
            return;
        }

        clearScreen();

        printf("====================================\n");
        printf("         SUSPECT INTERVIEWS         \n");
        printf("====================================\n");

        printf ("\n%s:\n", currentCase->suspect[index].name);
        printf ("   Q: %s\n", currentCase->suspect[index].questions[choice - 1]);
        printf ("   A: %s\n", currentCase->suspect[index].answers[choice - 1]);

        currentCase->suspect[index].asked[choice - 1] = 1;

        currentCase->suspect[index].interviewed = 1;

        pauseScreen();
    }
}

void reviewNotebook(const Case *currentCase) {
    int choice = 0;

    while (1) {
        clearScreen();
        printf ("====================================\n");
        printf ("         DETECTIVE NOTEBOOK         \n");
        printf ("====================================\n");

        printf ("   1. Collected Evidence\n");
        printf ("   2. Suspect Statements\n");
        printf ("   3. Return to Case Menu\n");
        printf ("\nChoice: ");

        choice = readNumber(1, 3);

        if (choice == 3) {
            return;
        }

        switch (choice) {
            case 1:
                reviewEvidence(currentCase);
                break;
            case 2:
                reviewStatements (currentCase);
                break;
        }
    }
}

void reviewEvidence(const Case *currentCase) {
    int counter = 0;

    clearScreen();

    printf ("====================================\n");
    printf ("         COLLECTED EVIDENCE         \n");
    printf ("====================================\n");

    for (int i = 0; i < currentCase->evidenceCount; i++) {
        if (currentCase->evidence[i].collected == 1) {
            printf ("\n%d. [%s]\n", counter + 1, currentCase->evidence[i].name);
            printf ("%s\n", currentCase->evidence[i].description);

            counter++;
        }
    }
    if (counter == 0) {
        printf ("\nNo evidence recorded yet.\n");
        pauseScreen();
        return;

    }
    printf ("\n");

    pauseScreen();
}

void reviewStatements(const Case *currentCase) {
    int counter = 0;

    clearScreen();

    printf ("====================================\n");
    printf ("         SUSPECT STATEMENTS         \n");
    printf ("====================================\n");

    for (int i = 0; i < currentCase->suspectCount; i++) {
        if (currentCase->suspect[i].interviewed == 1) {
            printf ("\n%d. [%s]\n\n", counter + 1, currentCase->suspect[i].name);
            printf ("Alibi:\n%s\n\n", currentCase->suspect[i].alibi);
            printf ("Statements:\n");

            for (int j = 0; j < currentCase->suspect[i].qaCount; j++) {
                if (currentCase->suspect[i].asked[j] == 1) {
                    printf ("   Q: %s\n", currentCase->suspect[i].questions[j]);
                    printf ("   A: %s\n\n", currentCase->suspect[i].answers[j]);
                }
            }
        counter++;
        }
    }

    if (counter == 0) {
        printf ("\nNo suspect statements has been recorded yet.\n");
        pauseScreen();

        return;
    }

    pauseScreen();
}

int makeAccusation (const Case *currentCase) {
    int choice = 0, collected = 0, interviewed = 0, total_qa = 0;

    clearScreen();

    printf ("====================================\n");
    printf ("          FINAL ACCUSATION          \n");
    printf ("====================================\n");

    printf ("\nYou have reached the end of your investigation.\n");
    printf ("Review your evidence carefully before proceeding.\n\n");

    for (int i = 0; i < currentCase->evidenceCount; i++) {
        if (currentCase->evidence[i].collected == 1) {
            collected++;
        }
    }

    for (int i = 0; i < currentCase->suspectCount; i++) {
        for (int j = 0; j < currentCase->suspect[i].qaCount; j++) {
            if (currentCase->suspect[i].asked[j] == 1) {
                interviewed++;
            }
            total_qa++;
        }
    }

    printf ("Collected Evidence: %d/%d\n", collected, currentCase->evidenceCount);
    printf ("Questions asked: %d/%d\n", interviewed, total_qa);
    printf ("\nWho do you believe is responsible?\n\n");

    for (int i = 0; i < currentCase->suspectCount; i++) {
        printf ("   %d. %s\n", i+1, currentCase->suspect[i].name);
    }

    printf ("\n   %d. Cancel Accusation\n", currentCase->suspectCount + 1);
    printf ("\nMake your accusation: ");

    choice = readNumber (1, currentCase->suspectCount + 1);

    if (choice == currentCase->suspectCount + 1) {
        return 0;
    }

    revealSolution(currentCase, choice - 1);

    return 1;
}

void revealSolution (const Case *currentCase, int index) {
    clearScreen();

    printf ("You have accused %s\n", currentCase->suspect[index].name);

    pauseScreen();
    clearScreen();

    printf ("===================================\n");
    printf ("           CASE SOLUTION           \n");
    printf ("===================================\n");
    printf ("%s\n", currentCase->solution);

    pauseScreen();

    if (strcmp(currentCase->suspect[index].name, currentCase->culprit) == 0) {
        clearScreen();
        printf("====================================\n");
        printf("             GOOD ENDING            \n");
        printf("====================================\n");

        printf("\nEXCELLENT DETECTIVE WORK!\n");

        printf("You identified %s and collected enough\n", currentCase->culprit);
        printf("evidence to expose their false alibi.\n");

        printf("\nThe case is officially solved.\n");

    } else {
        clearScreen();
        printf("====================================\n");
        printf("             BAD ENDING             \n");
        printf("====================================\n");

        printf("\nYour accusation was incorrect. %s escapes!\n", currentCase->culprit);

        printf("The real killer remains exposed by the evidence,\n");
        printf("but you failed to identify the right suspect.\n");
    }

    printf ("\n\nThank you for playing The Verdict!\n\n");

    pauseScreen();
}

void caseFiles (Case files[]) {
    files[0] = (Case) {
        .casename = "The Last Piece",

        .casebriefing =
            "\nVICTIM: Alexander Blackwood\n"
            "\nLOCATION: Alexander's Study\n"
            "\nTIME DISCOVERED: 10:00 PM\n"
            "\nSuspects: Five\n"

            "\nSUMMARY:\n"

            "Alexander Blackwood was a wealthy collector and historian who had recently discovered\n"
            "that someone had been falsifying historical ownership documents connected to his private collection.\n"

            "\nBefore he could expose the truth, Alexander was found dead inside his study.\n"

            "\nFive individuals had reasons to want him silenced.\n"
            "Each has an alibi, but some of their statements do not match the physical evidence.\n"

            "\nYour mission is to investigate the study, examine six pieces of evidence,\n"
            "question all five suspects, and identify the real murderer.\n",

        .evidence = {
            {
                .name = "The Burned Letter",
                .description =
                    "A partially burned letter addressed to Alexander.\n"
                    "\nReadable text:\n"
                    "If you reveal what happened, you will destroy everything we built. You will regret it.\n",
                .collected = 0
            },
            {
                .name = "The Broken Pocket Watch",
                .description =
                    "An antique pocket watch stopped at 9:20 PM.\n"
                    "\nIts initials belong to Richard Beaumont.\n",
                .collected = 0
            },
            {
                .name = "The Torn Photograph",
                .description =
                    "An old photograph shows Alexander with Dr. Adrian Cross inside the historical archive.\n"
                    "\nThe back contains a handwritten note:\n"
                    "The original records must remain untouched.\n",
                .collected = 0,
            },
            {
                .name = "The Brass Key",
                .description =
                    "A key belonging to the mansion's secondary entrance.\n"
                    "\nThomas Reed, the head of security, normally keeps this key.\n",
                .collected = 0,
            },
            {
                .name = "The Torn Notebook Page",
                .description =
                    "A page from Alexander's personal notebook.\n"
                    "\nIt reads:\n"
                    "The ownership records are false. A.C. changed the original documentation.\n"
                    "I must confront him before the originals disappear.\n",
                .collected = 0,
            },
            {
                .name = "The Dark Fabric Fiber",
                .description =
                    "A dark wool fiber is found caught on the armchair. It contains traces of restoration wax.",
                .collected = 0,
            }
        },

        .suspect = {
            {
                .name = "Eleanor Blackwood",

                .alibi =
                    "Claims she was in the west sitting room.",

                .questions =
                {
                    "What was your relationship with Alexander like recently?",
                    "Did you write the burned letter?",
                    "Where were you at approximately 9:30 PM?"
                },

                .answers =
                {
                    "We had disagreements, Detective. Alexander was becoming increasingly secretive about his affairs.",
                    "Yes. I was angry with him, but I never intended to hurt my husband.",
                    "I was in the west sitting room. I stayed there because I did not want another argument."
                },

                .asked =
                {
                    0,
                    0,
                    0
                },
                .interviewed = 0,
                .qaCount = 3
            },
            {
               .name = "Richard Beaumont",

                .alibi =
                    "Claims he was in the dining hall around 9:27 PM.",

                .questions =
                {
                    "What happened between you and Alexander?",
                    "Is the broken pocket watch yours?",
                    "Where were you around 9:30 PM?"
                },

                .answers =
                {
                    "We disagreed about our business partnership. He wanted to end it, and naturally, I was upset.",
                    "Yes, those are my initials. But I lost that watch days ago. I searched everywhere for it.",
                    "I was in the dining hall. I remember checking the time because I was waiting for the others."

                },
                .asked =
                {
                    0,
                    0,
                    0
                },
                .interviewed = 0,
                .qaCount = 3
            },
            {
               .name = "Victoria Hale",

                .alibi =
                    "Claims she was organizing records in the office.",

                .questions =
                {
                    "Did Alexander tell you about the falsified documents?",
                    "Have you seen the photograph found beneath the bookshelf?",
                    "Where were you during the incident?"
                },
                .answers =
                {
                    "He mentioned that some historical records were suspicious. He was planning to investigate them further.",
                    "Yes. I remember seeing it in the archive. Dr. Cross and Mr. Blackwood worked together on historical records.",
                    "I was in the office organizing documents. I did not enter the study that evening."

                },
                .asked =
                {
                    0,
                    0,
                    0
                },
                .interviewed = 0,
                .qaCount = 3
            },
            {
               .name = "Thomas Reed",

                .alibi =
                   "Claims he was monitoring the eastern corridor.",

                .questions =
                {
                    "Who has access to the secondary entrance?",
                    "Why was your brass key found near the study?",
                    "Did you notice anyone entering the study"
                },

                .answers =
                {
                    "I am responsible for its security. Only authorized personnel should have access to that key.",
                    "That is impossible. I kept my key with me throughout the evening. Someone must have taken it or placed another key there.",
                    "I did not see anyone use the main entrance to the study. The storm made it difficult to monitor the outside corridor."
                },
                .asked =
                {
                    0,
                    0,
                    0
                },
                .interviewed = 0,
                .qaCount = 3
            },
            {
               .name = "Dr. Adrian Cross",

                .alibi =
                    "Claims he was alone in the east gallery.",

                .questions =
                {
                    "What was your professional relationship with Alexander?",
                    "Did you visit Alexander's Study that evening?",
                    "Do you recognize the initials A.C. on the torn notebook page?"
                },

                .answers =
                {
                    "We collaborated on historical research. He was a respected collector, although we\n"
                    "occasionally disagreed about the authenticity of certain documents.",

                    "No. I remained in the east gallery. I had no reason to enter his private study.",
                    "Many people could have those initials. You cannot accuse someone based on two letters."
                },
                .asked =
                {
                    0,
                    0,
                    0
                },
                .interviewed = 0,
                .qaCount = 3
            }
        },

        .evidenceCount = 6,
        .suspectCount = 5,
        .culprit = "Dr. Adrian Cross",
        .solution =
            "The murderer is Dr. Adrian Cross.\n"

            "\nHis motive:\n"
            "Adrian had been forging historical ownership documents to make certain valuable artifacts\n"
            "appear to belong to him or to support false ownership claims.\n"
            "\nAlexander discovered the falsification and planned to expose him.\n"
            "\nAdrian realized that revealing the truth would destroy his career and reputation. He decided to silence Alexander.\n"

            "\nHow the murder happened:\n"
            "1. Adrian learned that Alexander had discovered the forged records.\n"
            "2. During the storm, Adrian used the secondary entrance to reach the study.\n"
            "3. He confronted Alexander and killed him.\n"
            "4. During the struggle, a fiber from his dark wool coat was caught on the armchair.\n"
            "5. He attempted to redirect suspicion by leaving evidence associated with other suspects, including the brass key.\n"
            "6. He left the study, believing that his alibi would protect him.\n"
    };
}
