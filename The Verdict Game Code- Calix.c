#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//GAME CONSTANTS
#define MAX_CASES 1
#define MAX_EVIDENCE 4
#define MAX_SUSPECT 3
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
    int choice = 0;

    while (1) {
        clearScreen();

        printf("====================================\n");
        printf("            SUSPECT LIST            \n");
        printf("====================================\n");

        for (int i = 0; i < currentCase->suspectCount; i++) {
            printf("   %d. %s\n", i+1, currentCase->suspect[i].name);
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
                printf (" [INTERVIEWED]");
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
    int choice = 0;

    clearScreen();

    printf ("====================================\n");
    printf ("          FINAL ACCUSATION          \n");
    printf ("====================================\n");

    printf ("\nYou have reached the end of your investigation.\n");
    printf ("Review your evidence carefully before proceeding.\n\n");

    printf ("Who do you believe is responsible?\n");

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
        .casename = "The Last Toast",

        .casebriefing =
            "\nVICTIM: Mr. Theodore Ashford\n"
            "\nLOCATION: Ashford Estate - Private Dining Room\n"
            "\nTIME DISCOVERED: 11:30 PM\n"

            "\nSUMMARY:\n"

            "Mr. Theodore Ashford, a wealthy businessman, was found dead\n"
            "in his private dining room during a small gathering at his estate.\n"

            "\nHe was discovered slumped over his chair, with a shattered wineglass beside him\n"
            "Initial investigations suggest that he was poisoned during dinner.\n"

            "\nThere were only two guests who remained at the estate that evening.\n"
            "Both had personal disagreements with the victim, and both claim\n"
            "to have had nothing to do with his death.\n"

            "\nHowever, inconsistencies in their statements suggest that someone may be hiding something.\n"

            "\nYour objective is to investigate the available evidence, question both suspects,\n"
            "examine their answers, and determine who murdered Theodore Ashford.\n",

        .evidence = {
            {
                .name = "The Broken Watch",
                .description =
                    "A silver pocket watch was found beneath the dining table.\n"
                    "\nIts hands were stopped at 10:45 PM. The watch belonged to the victim,\n"
                    "\nbut its glass was cracked, suggesting it had fallen during a struggle.",
                .collected = 0

            },
            {
                .name = "The Poisoned Glass",
                .description =
                    "A wineglass was found beside the victim. Laboratory testing detected traces of poison.\n"
                    "\nInterestingly, the other glasses on the table contained no traces of the substance.",
                .collected = 0

            },
            {
                .name = "The Threatening Note",
                .description =
                    "A handwritten note was discovered inside the victim's coat pocket. It reads:\n"
                    "\nYou cannot keep what rightfully belongs to me. Tonight, everything ends.",
                .collected = 0

            },
            {
                .name = "The Missing Glove",
                .description =
                    "A single black leather glove was found near the dining room entrance.\n"
                    "\nIt appears to belong to someone with unusually small hands.\n"
                    "\nA faint chemical smell was detected on its fingertips.",
                .collected = 0
            }
        },

        .suspect = {
            {
                .name = "Victor Hale",

                .alibi =
                    "I was in the mansion's library reviewing business documents from 10:00 PM until 11:15 PM.\n"
                    "\nI've never entered the dining room after dinner.",

                .questions =
                {
                    "What was your relationship with Theodore Ashford?",
                    "Where were you between 10:00 PM and 11:15 PM?",
                    "Did you know about the threatening note found in Theodore's coat?"
                },

                .answers =
                {
                    "We were business partners for nearly ten years. We had disagreements recently,\n"
                    "but nothing that would make me want him dead.",

                    "In the library. I was reviewing financial documents. I barely left my seat during that time.",

                    "No. Although I wouldn't be surprised if someone wanted to threaten him.\n"
                    "Theodore had many enemies, especially in business."
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
               .name = "Eleanor Whitmore",

                .alibi =
                    "I was in the upstairs guest room organizing the Theodore's correspondence.\n"
                    "\nI left the dining room at 10:15 PM and didn't return.",

                .questions =
                {
                    "How long had you worked for Theodore Ashford?",
                    "Why did you leave the dining room at 10:15 PM?",
                    "Do you recognize the black leather glove found near the dining room?"
                },

                .answers =
                {
                    "Almost five years. I handled his appointments, correspondence, and personal affairs.\n"
                    "\nI knew his habits better than anyone.",

                    "Mr. Ashford asked me to retrieve some documents from his office.\n"
                    "\nAfter that, I went upstairs to organize his correspondence.",

                    "No. I don't wear leather gloves. Besides, I was upstairs for most of the evening."
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
        .evidenceCount = 4,
        .suspectCount = 2,
        .culprit = "Eleanor Whitmore",
        .solution =
            "The poison was administered specifically to the victim's wineglass,\n"
            "\nindicating that the murderer knew which glass belonged to him.\n"

            "\nThe glove contains traces of the poison, suggesting that the murderer handled the substance.\n"

            "\nEleanor had access to the victim's personal belongings and knew his dining habits.\n"

            "\nHer claim that she left at 10:15 PM is suspicious because the victim's watch stopped at 10:45 PM, "
            "\npotentially indicating a later struggle.\n"

            "\nThe threatening note was deliberately planted to redirect suspicion toward Victor, "
            "\nwho had a known business dispute with the victim.\n"

            "\n\nMOTIVE: "
            "\nEleanor discovered that Theodore intended to dismiss her and "
            "\nexpose her involvement in embezzling company funds. "
            "\n\nFearing imprisonment and the loss of everything she had built, "
            "\nshe decided to kill him before he could reveal the truth."
    };
}

