#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <ctype.h>
#include "quiz_game.h"

#define MAX_QUESTIONS 10
#define MAX_HINTS 3

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define CYAN "\x1b[36m"
#define LIGHT_CYAN "\x1b[96m"
#define RESET "\x1b[0m"

// Function declarations
int loadQuestions(Question q[], const char *filename);
int playQuiz(Question q[], int total);
void saveScore(LeaderboardEntry entry);
void clearScreen();
void line();
void takeTest();
void displayMenu();
void viewLeaderboard();
int compareScores(const void *a, const void *b);

int main() {

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    int choice = 0;

    while (1) {
        displayMenu();
        printf(LIGHT_CYAN "Enter your choice: " RESET);
        if(scanf("%d", &choice)!=1) {
            while(getchar()!='\n'); // flush invalid input
            printf(RED "Invalid input! Try again.\n" RESET);
            continue;
        }
        while(getchar()!='\n'); // flush newline

        switch (choice) {
            case 1:
                takeTest();
                break;
            case 2:
                viewLeaderboard();
                break;
            case 3:
                printf(GREEN "Thank you for using MCQ Program!\n" RESET);
                exit(0);
            default:
                printf(RED "Invalid choice! Please try again.\n" RESET);
        }
    }
}

void displayMenu() {
    clearScreen();
    printf(CYAN "\n========================================\n" RESET);
    printf(GREEN "           MCQ QUIZ PROGRAM\n" RESET);
    printf(CYAN "========================================\n" RESET);
    printf(YELLOW "1. Take Test\n" RESET);
    printf(YELLOW "2. View Leaderboard\n" RESET);
    printf(YELLOW "3. Exit\n" RESET);
    printf(CYAN "========================================\n" RESET);
}

void takeTest() {
    int choice;
    char playAgain;
    Question questions[MAX_QUESTIONS];
    time_t start_time, end_time;
    LeaderboardEntry entry;
    const char *filename;

    do {
        clearScreen();
        printf(CYAN "\n====================================================\n" RESET);
        printf(GREEN "           WELCOME TO MULTI-SUBJECT QUIZ GAME        \n" RESET);
        printf(CYAN "====================================================\n\n" RESET);

        printf(LIGHT_CYAN "Enter your name: " RESET);
        fgets(entry.name, 50, stdin);
        entry.name[strcspn(entry.name, "\n")] = 0;

        printf(YELLOW "Choose a Subject:\n" RESET);
        printf(YELLOW "1. English\n2. Mathematics\n3. General Knowledge\n4. Geography\n" RESET);
        printf(LIGHT_CYAN "\nEnter your choice (1-4): " RESET);
        if(scanf("%d", &choice)!=1) {
            while(getchar()!='\n');
            printf(RED "Invalid input! Returning to menu.\n" RESET);
            return;
        }
        while(getchar()!='\n');

        switch (choice) {
            case 1:
                strcpy(entry.subject, "English");
                filename = "english.txt";
                break;
            case 2:
                strcpy(entry.subject, "Mathematics");
                filename = "maths.txt";
                break;
            case 3:
                strcpy(entry.subject, "General Knowledge");
                filename = "general_knowledge.txt";
                break;
            case 4:
                strcpy(entry.subject, "Geography");
                filename = "geography.txt";
                break;
            default:
                printf(RED "Invalid choice! Try again.\n" RESET);
                continue;
        }

        int total = loadQuestions(questions, filename);
        if (total == 0) {
            printf(RED "\nError: No questions found or file missing!\n" RESET);
            continue;
        }

        start_time = time(NULL);
        int score = playQuiz(questions, total);
        end_time = time(NULL);

        entry.score = score;
        entry.total_questions = total < MAX_QUESTIONS ? total : MAX_QUESTIONS;
        entry.test_date = end_time;
        entry.time_taken = (int)difftime(end_time, start_time);
        saveScore(entry);

        printf(LIGHT_CYAN "\nDo you want to play again? (y/n): " RESET);
        scanf(" %c", &playAgain);
        while(getchar()!='\n');

    } while (playAgain == 'y' || playAgain == 'Y');

    printf(GREEN "\nThank you for playing! Goodbye!\n" RESET);
}

int loadQuestions(Question q[], const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;

    int i = 0;
    while (i < MAX_QUESTIONS && fgets(q[i].question, sizeof(q[i].question), fp)) {
        fgets(q[i].options[0], sizeof(q[i].options[0]), fp);
        fgets(q[i].options[1], sizeof(q[i].options[1]), fp);
        fgets(q[i].options[2], sizeof(q[i].options[2]), fp);
        fgets(q[i].options[3], sizeof(q[i].options[3]), fp);

        fscanf(fp, " %c\n", &q[i].correctAnswer);  // now expects A/B/C/D
        fgets(q[i].hint, sizeof(q[i].hint), fp);

        char separator[10];
        fgets(separator, sizeof(separator), fp);  // skip ---
        i++;
    }

    fclose(fp);
    return i;
}

int playQuiz(Question q[], int total) {
    clearScreen();
    int score = 0, hintsUsed = 0;
    char answer, useHint;
    char optionLabels[] = {'A', 'B', 'C', 'D'};

    for (int i = 0; i < total; i++) {
        clearScreen();
        line();
        printf(BLUE "Question %d of %d\n" RESET, i + 1, total);
        line();

        printf(LIGHT_CYAN "%s\n" RESET, q[i].question);
        for (int j = 0; j < 4; j++) {
            printf(YELLOW "%c. %s" RESET, optionLabels[j], q[i].options[j]);
        }

        if (hintsUsed < MAX_HINTS) {
            printf(CYAN "Would you like a hint? (y/n): " RESET);
            scanf(" %c", &useHint);
            while(getchar()!='\n');
            if (useHint == 'y' || useHint == 'Y') {
                printf(GREEN "%s\n" RESET, q[i].hint);
                hintsUsed++;
            }
        }

        printf(LIGHT_CYAN "\nEnter your answer (A/B/C/D): " RESET);
        scanf(" %c", &answer);
        while(getchar()!='\n');

        if (toupper(answer) == toupper(q[i].correctAnswer)) {
            score += 1;  // each correct answer gives 1 point
        }

        if (i < total - 1) {
            printf(LIGHT_CYAN "\nPress Enter for next question..." RESET);
            getchar();
        }
    }

    clearScreen();
    printf(CYAN "\n============================================\n" RESET);
    printf(GREEN "              QUIZ COMPLETED!\n" RESET);
    printf(CYAN "============================================\n" RESET);
    printf(YELLOW "Your Total Score: %d / %d\n" RESET, score, total);
    printf(YELLOW "Hints Used: %d / %d\n" RESET, hintsUsed, MAX_HINTS);

    return score;
}

void saveScore(LeaderboardEntry entry) {
    FILE *fp = fopen("leaderboard.dat", "ab");
    if (!fp) {
        printf(RED "Error saving score!\n" RESET);
        return;
    }

    fwrite(&entry, sizeof(LeaderboardEntry), 1, fp);
    fclose(fp);

    printf(GREEN "\nYour score has been saved!\n" RESET);
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void line() {
    printf(CYAN "----------------------------------------------------\n" RESET);
}

void viewLeaderboard() {
    clearScreen();
    FILE *fp = fopen("leaderboard.dat", "rb");
    LeaderboardEntry entries[1000];
    int count = 0;
    char time_str[26];

    if (!fp) {
        printf(RED "No leaderboard data found!\n" RESET);
        return;
    }

    while (fread(&entries[count], sizeof(LeaderboardEntry), 1, fp) == 1) count++;
    fclose(fp);

    if (count == 0) {
        printf(RED "Leaderboard is empty!\n" RESET);
        return;
    }

    qsort(entries, count, sizeof(LeaderboardEntry), compareScores);

    printf(CYAN "\n=========================================================================================\n" RESET);
    printf(GREEN "          LEADERBOARD\n" RESET);
    printf(CYAN "=========================================================================================\n" RESET);
    printf(YELLOW "%-4s %-20s %-20s %-10s %-10s %-20s\n" RESET,
           "Rank", "Name", "Subject", "Score", "Time(s)", "Date");
    printf(CYAN "=========================================================================================\n" RESET);

    for (int i = 0; i < count; i++) {
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S",
                 localtime(&entries[i].test_date));
        char score_str[20];
        sprintf(score_str, "%d/%d", entries[i].score, entries[i].total_questions);
        printf(YELLOW "%-4d %-20s %-20s %-10s %-10d %-20s\n" RESET,
               i + 1,
               entries[i].name,
               entries[i].subject,
               score_str,
               entries[i].time_taken,
               time_str);
    }
    printf(CYAN "=========================================================================================\n" RESET);
    printf(LIGHT_CYAN "\nPress Enter to return to menu..." RESET);
    getchar();
}

int compareScores(const void *a, const void *b) {
    LeaderboardEntry *entryA = (LeaderboardEntry *)a;
    LeaderboardEntry *entryB = (LeaderboardEntry *)b;

    float percentA = (float)entryA->score / entryA->total_questions;
    float percentB = (float)entryB->score / entryB->total_questions;

    if (percentB > percentA) return 1;
    if (percentB < percentA) return -1;

    return entryA->time_taken - entryB->time_taken;
}
