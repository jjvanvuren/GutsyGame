/*
 * Student No:		C3252194
 * Name:			Jacobus Janse van Vuren
 * Program:			Gutsy
 * Lecturer:		Dr Cesar Sanin
 * Date:			26-05-2017
 *
 * Description:		This program plays one or more games of Gutsy and displays the turn, game and
 *					final results to the console. At the end of every game, statistics are calculated
 *					using the game results and displayed to the console for the user to see. Statistics
 *					for all games is also shown once the desired number of games have been played.
 *					The statistics contains the probability of rolling each number on a 6 sided die, the 
 *					average number of times the players rolled in a turn, the probability of each player winning
 *					and the average number of rolls required to win the game (accumulate 101 or more points).
 *					This statistical data can then be analised to help determine the best risk factor to
 *					use (amount of points to stop rolling at in a turn) in order to have the highest probability
 *					of winning.
 */

#include	"stdafx.h"				/* printf_s and scanf_s	functions */
#include	<math.h>				/* round function */
#include	<string.h>				/* _strupr_s and strcpy_s functions */
#include	<stdlib.h>				/* abs and srand functions */
#include	<time.h>				/* time_t time function */

#define		WIN_VALUE	101			/* The value the player must reach to win the game */
#define		MAX_PLAYERS 20			/* maximum number of players per game */
#define		NAME_CHAR	20			/* maximum characters that can be used in a player name */
#define		MAX_GAMES	1001		/* maximum number of games that can be played in one run */
#define		SENTINEL	-1			/* sentinel value used to end loops */

/* Structure type used to store player data */
typedef struct {
	char	playerName[NAME_CHAR];	/* string of player name */
	int		stopValue;				/* the designated points to stop rolling at (as chosen by player) */
	int		totalRolls;				/* amount of times player rolled in a turn */
	int		totalTurns;				/* total turns a player took in a game */
	int		turnPoints;				/* points the player has accumulated in one turn */
	int		totalPoints;			/* total points a player has accumulated in a game */
	int		winCheck;				/* used to check if the player has won (reached 101 points) */
	int		totalWins;				/* The total number of times the player won for all games played */
	int		grandTotalRolls;		/* the total rolls for a player for all games played */
	int		grandTotalTurns;		/* the total turns a player took for all games played */
} playerData_t;

/* Structure type used to store game statistics */
typedef struct {
	double	probRoll1;				/* contains either the total times player rolled 1 or the probability of rolling a 1 */
	double	probRoll2;				/* contains either the total times player rolled 2 or the probability of rolling a 2 */
	double	probRoll3;				/* contains either the total times player rolled 3 or the probability of rolling a 3 */
	double	probRoll4;				/* contains either the total times player rolled 4 or the probability of rolling a 4 */
	double	probRoll5;				/* contains either the total times player rolled 5 or the probability of rolling a 5 */
	double	probRoll6;				/* contains either the total times player rolled 6 or the probability of rolling a 6 */
	int		averageGet101;			/* contains the number of times the winning player rolled to reach 101 */

} statistics_t;

/* Function Signatures */
void gutsyInstructions(void);
void diceThrown(int dNumber, int *accumValue, int *numTimes, int *count1, int *count2,
	int *count3, int *count4, int *count5, int *count6);
int playerTurn(statistics_t inStats, int player, int dNumber, int accumValue, int accumRolls, int *playerTotal,
	int *playerTurnPoints, int *playerRolls, statistics_t *outStats);
void playGutsy(void);
void playerPrint(int player, char playerName[], int dNumber, int accumValue, int playerTotal);
void displayTable(int gameNo, int numPlayers, playerData_t player[]);
statistics_t statistics(statistics_t gStats, statistics_t tStats);
void statisticsTotal(int gameNo, int numGames, int numPlayers, playerData_t info[], statistics_t stats);
void sortPlayers(char(*names)[MAX_PLAYERS], int numPlayers);

int
main(void)
{
	/* Seeds the number generator */
	time_t seconds;
	time(&seconds);
	srand((unsigned int)seconds);

	/* Display Gutsy instructions */
	gutsyInstructions();

	/* Run the playGutsy function */
	playGutsy();

	return(0);
}

/*
 * This function rolls a single die and returns the accumulated value. The exception being if
 * the player throws 1 it returns an accumulated value of 0.
 * input		- The "defined number" as chosen by the player
 * outputs	- The sum of the dice thrown, the number of times it was thrown, and the amount of
 * times the player threw a 1, 2, 3, 4, 5, and 6 before reaching their defined number.
 */
void
diceThrown(int dNumber, int *accumValue, int *numTimes, int *count1, int *count2, int *count3,
	int *count4, int *count5, int *count6)
{
	int check = 0,		/* process - used to stop rolling once a 1 is rolled */
		roll,			/* process - a random number between 1 and 6 */
		aValue = 0,		/* process - counter used for *accumValue */
		nTimes = 0,		/* process - counter used for *numTimes */
		c1 = 0,			/* process - counter used for *count1 */
		c2 = 0,			/* process - counter used for *count2 */
		c3 = 0,			/* process - counter used for *count3 */
		c4 = 0,			/* process - counter used for *count4 */
		c5 = 0,			/* process - counter used for *count5 */
		c6 = 0;			/* process - counter used for *count6 */

						/* Loop to roll die until dNumber is reached or roll is 1 */
	while (aValue < dNumber && check != SENTINEL) {

		/* Generate random number to simulate die roll */
		roll = rand() % 6 + 1;

		if (roll == 6) { /* If player rolls a 6, add 6 to aValue and increment c6 by 1 */
			aValue = aValue + 6;
			c6++;
		} else if (roll == 5) { /* If player rolls a 5, add 5 to aValue and increment c5 by 1 */
			aValue = aValue + 5;
			c5++;
		} else if (roll == 4) { /* If player rolls a 4, add 4 to aValue and increment c4 by 1 */
			aValue = aValue + 4;
			c4++;
		} else if (roll == 3) { /* If player rolls a 3, add 3 to aValue and increment c3 by 1 */
			aValue = aValue + 3;
			c3++;
		} else if (roll == 2) { /* If player rolls a 2, add 2 to aValue and increment c2 by 1 */
			aValue = aValue + 2;
			c2++;
		} else { /* If player rolls a 1, set aValue to 0, increment c1 by 1 and end the loop */
			aValue = 0;
			check = SENTINEL;
			c1++;
		}

		/* Increment number of die rolls by 1 */
		nTimes++;
	}

	/* Assign the total values accumulated in the variables to their corresponding pointers */
	*accumValue = aValue;
	*numTimes = nTimes;
	*count1 = c1;
	*count2 = c2;
	*count3 = c3;
	*count4 = c4;
	*count5 = c5;
	*count6 = c6;
}

/*
 * This function prompts the user for information required to simulate multiple games of Gutsy.
 * It requests the amount of players, amount of games, player names and their stop values.
 * playGutsy then stores statistical data for each game in structures.
 * Appropriate information is then printed to the console for every turn/round/game. The function
 * also displayes a table of statistics used to calculate the best stop value at the end of all games.
 */
void
playGutsy(void)
{
	int	numPlayers;				/* input - number of players to play Gutsy */
	int	numGames;				/* input - number of games to be played */
	char stringName[NAME_CHAR];		/* input - firstname of player */
	int stop;					/* input - number of points to stop at in turn */
	int p;						/* process - counter for players */
	int t;						/* process - counter for players */
	int r;						/* process - counter for players */
	int playerRolls;			/* process - number of rolls for player in a turn */
	int	winCheck = 0;			/* process - used to determine if a player has won */
	int curWinVal = 0;			/* process - containes total points accumulated by current winner */
	int curWinner;				/* process - player number of the current winner */
	int winner;					/* process - player number of the game winner */
	int	totalCheck = 0;			/* process - checks if player has reached 101 or more points */
	int gameNo;					/* process - the number of the current game */

	playerData_t playerInfo[MAX_PLAYERS];	/* process - contains information about players */
	statistics_t gameStats;					/* process - contains statistics for a single game */
	statistics_t totalStats;				/* process - contains final statistics for all games */

	printf_s("		***** LET'S PLAY GUTSY *****\n\n");

	/* Prompt user for amount of players */
	printf_s("Please type in the number of players: ");
	scanf_s("%d", &numPlayers);

	/* Check if number of players is valid */
	while (numPlayers < 2 || numPlayers > MAX_PLAYERS) {
		printf_s("\n### ERROR ###\n");
		printf_s("Players must be more than 1 and less than %d!\n\n", (MAX_PLAYERS + 1));
		printf_s("Please type in the number of players: ");
		scanf_s("%d", &numPlayers);
	}

	/* Prompt user for amount of games */
	printf_s("\nPlease type in the number of games: ");
	scanf_s("%d", &numGames);

	/* Check if number of games is valid */
	while (numGames < 1 || numGames > MAX_GAMES) {
		printf_s("\n### ERROR ###\n");
		printf_s("Number of games must be more than 0 and less than %d!\n\n", (MAX_GAMES + 1));
		printf_s("Please type in the number of players: ");
		printf_s("\nPlease type in the number of games: ");
		scanf_s("%d", &numGames);
	}

	/* Ask for player names and minimum number to stop rolling ("defined number" - risk factor) */
	for (p = 0; p < numPlayers; p++) {

		/* Ask for player name */
		printf_s("Player %d:\n", (p + 1));
		printf_s("		Name: ");
		scanf_s("%s", stringName, NAME_CHAR);

		/* Change name to all capital letters */
		_strupr_s(stringName);

		/* Add player's name to player's structure */
		strcpy_s(playerInfo[p].playerName, stringName);

		/* Ask for player's defined number */
		printf_s("		Minimum number to stop in a turn: ");
		scanf_s("%d", &stop);

		/* Add player's defined number to player's structure */
		playerInfo[p].stopValue = stop;

		/* Set player info to zero */
		playerInfo[p].totalPoints = 0;
		playerInfo[p].totalRolls = 0;
		playerInfo[p].totalTurns = 0;
		playerInfo[p].turnPoints = 0;
		playerInfo[p].totalWins = 0;
		playerInfo[p].winCheck = 0;
		playerInfo[p].grandTotalRolls = 0;
		playerInfo[p].grandTotalTurns = 0;
	}

	/* set game stats to zero */
	gameStats.probRoll1 = 0;
	gameStats.probRoll2 = 0;
	gameStats.probRoll3 = 0;
	gameStats.probRoll4 = 0;
	gameStats.probRoll5 = 0;
	gameStats.probRoll6 = 0;
	gameStats.averageGet101 = 0;

	/* set round stats to zero */
	totalStats.probRoll1 = 0;
	totalStats.probRoll2 = 0;
	totalStats.probRoll3 = 0;
	totalStats.probRoll4 = 0;
	totalStats.probRoll5 = 0;
	totalStats.probRoll6 = 0;
	totalStats.averageGet101 = 0;

	printf_s("\n\n				***** GAME STARTING *****\n\n");


	gameNo = 1;	/* Set the first game to 1 */
	while (gameNo <= numGames) {	/* Loop for the desired amount of games */

									/* Print the current game number */
		printf_s("Game %d\n", gameNo);


		while (totalCheck < WIN_VALUE) { /* Loop for 1 game of Gutsy */


			t = 0; /* Reset the player counter */
			while (t < numPlayers && totalCheck < WIN_VALUE) { /* Loop for a round of Gutsy*/

															   /* Increment total player turns */
				playerInfo[t].totalTurns = playerInfo[t].totalTurns + 1;

				/* Simulate a player turn and store the turn information in their appropriate variables */
				winCheck = playerTurn(gameStats, t, playerInfo[t].stopValue, playerInfo[t].totalPoints, playerInfo[t].totalRolls,
					&playerInfo[t].totalPoints, &playerInfo[t].turnPoints, &playerRolls, &gameStats);

				/* Add whether or not the player has won to the players structure */
				playerInfo[t].winCheck = winCheck;

				/* Add the players rolls for the turn to their total rolls for the game */
				playerInfo[t].totalRolls = playerInfo[t].totalRolls + playerRolls;

				/* Print player information */
				playerPrint(t, playerInfo[t].playerName, playerInfo[t].stopValue, playerInfo[t].turnPoints, playerInfo[t].totalPoints);

				/* Check if the player has reached 101 or more points */
				totalCheck = playerInfo[t].totalPoints;

				/* Increment t for next player turn */
				t++;
			}

			/* Print round table */
			displayTable(gameNo, numPlayers, playerInfo);

			/* Reset the current winner */
			curWinner = 0;

			/* Set current winning value to the total points of the first player */
			curWinVal = playerInfo[0].totalPoints;

			/* Loop through the players and if their total points is higher than the previous players */
			/* set the player as the current winner */
			for (r = 0; r < numPlayers; r++) {
				if (playerInfo[r].totalPoints > curWinVal) {
					curWinVal = playerInfo[r].totalPoints;
					curWinner = r;
				}
			}

			/* Only display the current winner if they haven't yet reached 101 */
			if (curWinVal < WIN_VALUE) {
				printf_s("		***** Current Winner: %s *****\n\n", playerInfo[curWinner].playerName);
			}

			/* Reset the winner value */
			winner = 0;

			/* Loop through players and if the playerTurn function set their winCheck value to SENTINEL */
			/* set the player as the winner for the game */
			for (r = 0; r < numPlayers; r++) {
				if (playerInfo[r].winCheck == SENTINEL) {

					/* Increment the players total wins */
					playerInfo[r].totalWins = playerInfo[r].totalWins + 1;

					winner = r;

					/* Display the winner's name in the console window */
					printf_s("		***** Winner: %s *****\n\n", playerInfo[r].playerName);
				}
			}
		}

		/* Reset the value of winCheck for the next game */
		totalCheck = 0;

		/* Loop through all players */
		for (p = 0; p < numPlayers; p++) {

			/* Add the players total rolls for the game to their grand total rolls*/
			playerInfo[p].grandTotalRolls = playerInfo[p].grandTotalRolls + playerInfo[p].totalRolls;

			/* Add the players total turns for the game to their grand total turns*/
			playerInfo[p].grandTotalTurns = playerInfo[p].grandTotalTurns + playerInfo[p].totalTurns;
		}

		/* Add the game winner's total rolls to the structure containing the total rolls to reach 101 */
		gameStats.averageGet101 = playerInfo[winner].totalRolls;

		/* Add the current games statistic data to the final statistic data */
		totalStats = statistics(gameStats, totalStats);

		/* Display the statistics for the current game */
		statisticsTotal(gameNo, numGames, numPlayers, playerInfo, gameStats);

		for (p = 0; p < numPlayers; p++) {
			/* Set player info counters to zero */
			playerInfo[p].totalPoints = 0;
			playerInfo[p].totalRolls = 0;
			playerInfo[p].totalTurns = 0;
			playerInfo[p].turnPoints = 0;
			playerInfo[p].winCheck = 0;
		}

		/* Increment the game number */
		gameNo++;
	}

	/* After all games have been played, display the final statistics for all games */
	statisticsTotal(SENTINEL, numGames, numPlayers, playerInfo, totalStats);
}

/* 
 * A function that plays a turn for a player using the diceThrown function and accumulates values towards
 * the winning value of 101 and statistics to be displayed at the end of games
 * inputs - The statistics structure that stores data for the game, the player number, the player's designated number,
 *          the amount of points they have accumulated so far and the amount of times they have already rolled in the game
 * outputs - The players total points, the points accumulated in the turn, the amount of times the player rolled in the turn
 *			 and the statistics structure that includes the data for the current turn
 */
int
playerTurn(statistics_t inStats, int player, int dNumber, int accumValue, int accumRolls, int *playerTotal,
	int *playerTurnPoints, int *playerRolls, statistics_t *outStats)
{
	int c1;						/* process - the amount of 1s the player rolled */
	int c2;						/* process - the amount of 2s the player rolled */
	int c3;						/* process - the amount of 3s the player rolled */
	int c4;						/* process - the amount of 4s the player rolled */
	int c5;						/* process - the amount of 5s the player rolled */
	int c6;						/* process - the amount of 6s the player rolled */
	int nTimes;					/* process - the amount of times the player rolled the dice */
	int turnPoints;				/* output - points the player accumulated in the turn */
	int totalPoints;			/* output - the players total points accumulated in the game */
	statistics_t turnStats;		/* output - structure that holds the values to be used in the game's
								*          statistics calculations */

								/* Roll the die until player rolls 1, the designated value or the winning value */
	diceThrown(dNumber, &turnPoints, &nTimes, &c1, &c2, &c3, &c4, &c5, &c6);

	/* Add the turn's points to the player's total points */
	totalPoints = accumValue + turnPoints;

	/* Return the player's total points using a pointer */
	*playerTotal = totalPoints;

	/* Return the player's turn points using a pointer */
	*playerTurnPoints = turnPoints;

	/* Add the number of times a number was rolled in the turn to the turnStats */
	turnStats.probRoll1 = inStats.probRoll1 + c1;
	turnStats.probRoll2 = inStats.probRoll2 + c2;
	turnStats.probRoll3 = inStats.probRoll3 + c3;
	turnStats.probRoll4 = inStats.probRoll4 + c4;
	turnStats.probRoll5 = inStats.probRoll5 + c5;
	turnStats.probRoll6 = inStats.probRoll6 + c6;

	/* Return the statistics using a pointer */
	*outStats = turnStats;

	/* Return the amount of times the player rolled the die via pointer */
	*playerRolls = nTimes;

	/* If the player reached 101 points in the turn, return the
	* value of SENTINEL to indicate the player has won */
	if (totalPoints >= WIN_VALUE) {
		return(SENTINEL);
	}
	/* If the player hasn't won the game yet return 0 */
	else {
		return(0);
	}
}

/*
 * Function used to print information about a players turn to the console
 * inputs - The player's number, name, designated number,
 *			total points accumulated and points accumulated in the turn
 * output - The player's name, designated number, points obtained and the
 *			players total points
 */
void
playerPrint(int player, char playerName[], int dNumber, int accumValue, int playerTotal)
{
	/* Print information for the players turn to the console */
	printf_s("		Player %d - %s:\n", (player + 1), playerName);
	printf_s("			Minimum number to stop in a turn: %d\n", dNumber);
	printf_s("			Points Obtained: %d\n", accumValue);
	printf_s("			Total Points: %d\n\n", playerTotal);
}

/* 
 * Function prints information for a single round in table format to the console
 * inputs - The current game number, number of players and the player data structure array
 * output - The players names, turn points and total points
 */
void
displayTable(int gameNo, int numPlayers, playerData_t player[])
{
	int i; /* process - counter used to loop through players */

	/* Print the table header with current game number */
	printf_s("Game %d - Table\n", gameNo);
	printf_s("	Player		Turn Points		Total Points\n");

	/* print all players data in table */
	for (i = 0; i < numPlayers; i++) {
		printf_s("	%s		%d			%d\n", player[i].playerName, player[i].turnPoints, player[i].totalPoints);
	}
	printf_s("\n");
}

/* 
 * This function stores statistical data to be used at the end of games
 * inputs - structure containing info to calculate game statistics
 *			structure containing info to calculate total statistics
 * output - returns a statistics_t structure containing the games
 *          stats added to the total stats
 */
statistics_t
statistics(statistics_t gStats, statistics_t tStats)
{
	statistics_t outStats;	/* output - the new structure to be returned by function */

	/* Add the game's statistics to the total statistics */
	outStats.probRoll1 = tStats.probRoll1 + gStats.probRoll1;
	outStats.probRoll2 = tStats.probRoll2 + gStats.probRoll2;
	outStats.probRoll3 = tStats.probRoll3 + gStats.probRoll3;
	outStats.probRoll4 = tStats.probRoll4 + gStats.probRoll4;
	outStats.probRoll5 = tStats.probRoll5 + gStats.probRoll5;
	outStats.probRoll6 = tStats.probRoll6 + gStats.probRoll6;
	outStats.averageGet101 = tStats.averageGet101 + gStats.averageGet101;

	return(outStats);
}

/* This function calculates and prints the game/total statistics to the console.
 * The statistics include the probability of rolling a 1, 2, 3, 4, 5 and 6. Also included
 * is the average rolls per tern for the players (names sorted alphabetically), the probability
 * that the player wins and the average rolls required to win (get 101 points)
 * inputs - The current game number (SENTINEL if final statistics), the number of games,
 *			the number of players, player information and data required to calculate game/total
 *			statistics
 * output - prints game/total statistics to console
 */
void
statisticsTotal(int gameNo, int numGames, int numPlayers, playerData_t info[], statistics_t stats)
{
	double	p1,									/* output - the probability of rolling a 1 */
			p2,									/* output - the probability of rolling a 2 */
			p3,									/* output - the probability of rolling a 3 */
			p4,									/* output - the probability of rolling a 4 */
			p5,									/* output - the probability of rolling a 5 */
			p6,									/* output - the probability of rolling a 6 */
			pWin;								/* output - the probability of the player winning */
	double	avRollPerTurn;						/* output - player's average rolls per turn */
	double	pRolls;								/* process - the total number of rolls for all players */
	double	avRoll101;							/* output - the average rolls required to reach 101 points*/
	int		p;									/* process - counter for inner loop printing players av rolls */
	int		i;									/* process - counter for outer loop printing players av rolls*/
	char	playerName[MAX_PLAYERS][NAME_CHAR];	/* process - array containing player names to be sorted */
	int		cmp;								/* process - used to compaire player names */

	/* Add player names from playerData_t info to playerName array */
	for (p = 0; p < numPlayers; p++) {
		memcpy(playerName[p], info[p].playerName, NAME_CHAR);
	}

	/* Sort the player names */
	sortPlayers(playerName, numPlayers);

	/* Display the statistics header */
	if (gameNo == SENTINEL) {	/* If displaying the final statistics*/
		printf_s("Final - Statistics\n");
	} else {	/* If displaying the game's statistics */
		printf_s("Game %d - Statistics\n", gameNo);
	}

	/* Calculate the total rolls for all players */
	pRolls = stats.probRoll1 + stats.probRoll2 + stats.probRoll3 + stats.probRoll4 +
		stats.probRoll5 + stats.probRoll6;

	/* Calculate the probabilities of rolling each number */
	p1 = stats.probRoll1 / pRolls;
	p2 = stats.probRoll2 / pRolls;
	p3 = stats.probRoll3 / pRolls;
	p4 = stats.probRoll4 / pRolls;
	p5 = stats.probRoll5 / pRolls;
	p6 = stats.probRoll6 / pRolls;

	/* Print the probabilities of rolling each number */
	printf_s("		P(1) = %.4lf\n", p1);
	printf_s("		P(2) = %.4lf\n", p2);
	printf_s("		P(3) = %.4lf\n", p3);
	printf_s("		P(4) = %.4lf\n", p4);
	printf_s("		P(5) = %.4lf\n", p5);
	printf_s("		P(6) = %.4lf\n", p6);

	for (i = 0; i < numPlayers; i++) {	/* Loop through every player */

		pWin = 0;
		for (p = 0; p < numPlayers; p++) {	/* Loop through the sorted names for every player */

			/* Compare the current player's name to the current player name in the sorted array */
			cmp = strcmp(playerName[i], info[p].playerName);

			if (cmp == 0) {		/* If the names are the same */
				if (gameNo == SENTINEL) {	/* If this is the final statistics */

					/* Calculate the player's final average rolls per turn */
					avRollPerTurn = round(double(info[p].grandTotalRolls) / info[p].grandTotalTurns);

					/* Print the player's final average rolls per turn */
					printf_s("		%s - A(%d)	= %.0lf\n", info[p].playerName, info[p].stopValue, avRollPerTurn);

					/* Calculate the players probability of winning */
					pWin = double(info[p].totalWins) / numGames;

					/* Print the number of times the player won */
					printf_s("		P(WIN)		= %.4lf\n", pWin);

				} else { /* If this is a games statistics */

					/* Calculate the player's average rolls per turn for the game */
					avRollPerTurn = round(double(info[p].totalRolls) / info[p].totalTurns);

					/* Print the player's average rolls per turn for the game */
					printf_s("		%s - A(%d)	= %.0lf\n", info[p].playerName, info[p].stopValue, avRollPerTurn);
				}
			}
		}
	}
	if (gameNo == SENTINEL) {	/* If this is the final statistics */

		/* Calculate the average rolls required to reach 101 for all games */
		avRoll101 = round(double(stats.averageGet101) / numGames);

		/* Print the average rolls required to reach 101 for all games */
		printf_s("		A(101) = %.0lf\n\n", avRoll101);

	} else {	/* If this is a games statistics */

		/* The average rolls required to reach 101 for the game is the winning player's total rolls */
		avRoll101 = round(double(stats.averageGet101));

		/* Print the rolls for the winning player to reach 101 in the game */
		printf_s("		A(101) = %.0lf\n\n", avRoll101);
	}
}

/* A function that prints the instuctions for Gutsy to the console */
void
gutsyInstructions(void)
{
	/* Print the instuctions for Gutsy to the console */
	printf_s("		***** HOW TO PLAY GUTSY *****\n\n");
	printf_s("Objective: Be the first player to accumulate 101 points.\n");
	printf_s("Players:   This game can be played by 2 to 20 people at a time.\n\n");
	printf_s("Before starting a game of Gutsy each player needs to pick a minimum\n");
	printf_s("amount of points (risk factor) that must be accumulated before his/her\n");
	printf_s("turn ends.\n\n");
	printf_s("Players will then take turns rolling a single die to accumulate points.\n");
	printf_s("The player's turn ends once s/he reaches their risk factor, reaches 101\n");
	printf_s("points, or rolls a 1. If the player rolls a one, his/her turn ends and\n");
	printf_s("s/he gets no points that turn.\n");
	printf_s("Players keep taking turns until someone accumulates 101 or more points.\n\n");
	printf_s("This program simulates one or more games of Gutsy and displays a table\n");
	printf_s("of statistics at the end of each game.\n\n");
}

/* This function sorts an array of players names in place using the bubble sort method
 * inputs - the array containing player names and the number of players
 * output - the array containing player names is sorted alphabetically
 */
void
sortPlayers(char(*names)[MAX_PLAYERS], int numPlayers)
{
	int 	i,						/* Counter to loop for the number of players */
			j,						/* Counter to loop through the player names */
			cmp;					/* process - used to compare player names */
	char 	temp[NAME_CHAR];		/* process - temporary storage for player name */

	for (i = 0; i < numPlayers; i++) {	/* loop for the number of players */
		for (j = 0; j < numPlayers - 1; j++) {	/* loop through the player names minus 1 because
												 * the last word will already be sorted */
			/* Compare the current name in array to next name */
			cmp = strcmp(names[j], names[j + 1]);

			if (cmp > 0) {	/* if the next name is less (closer to a) than the current name in array */

				/* swap current and next player names in array */
				strcpy_s(temp, names[j + 1]); /* add the next word in array to temp */
				strcpy_s(names[j + 1], names[j]);	/* move the current word to the array pos of next word*/
				strcpy_s(names[j], temp);	/* add the temp name to the current array pos */
			}
		}
	}
}