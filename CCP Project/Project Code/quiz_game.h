typedef struct {
    char question[256];
    char options[4][128];
    char correctAnswer;
    char hint[200];
} Question;

typedef struct
{
	char name[50];
	char subject[50];
	int score;
	int total_questions;
	time_t test_date;
	int time_taken; // in seconds
} LeaderboardEntry;

