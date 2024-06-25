/* 
  Program: (Non Deterministic) Finite Automata
  Creator: Jessica Nguyen
  Date: 2023-09-29
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LINE_MAX 1000
#define STR_MAX 10

typedef struct active
{
    char state[STR_MAX+1];
    int active;
} active;

typedef struct transition
{
    char stateOne[STR_MAX+1];
    char transition[STR_MAX+1];
    char stateTwo[STR_MAX+1];
} transition;

char * fileReader(char fileName[]);
active * epsilonList(int stateNum, int transitionNum, transition *transitionList, active *list);

int main (int argc, char *argv[])
{   
    // Checks if an input was obtained from the command line
    if (argc == 2)
    {
        // Declaring variables
        active *currentList, *nextList;
        transition *transitionList;
        char *split, *entireFile, **state, **input, startState[STR_MAX+1], acceptState[STR_MAX+1];
        int i, j, k, x, alphabetNum, stateNum, inputNum, transitionNum;

        // Checks if multiple inputs were given at the command line (ex: ./a1 "ex1 ex2")
        for (i = 0; i < strlen(argv[1]); i++)
        {
            if (argv[1][i] == ' ')
            {
                printf("Too many files were given in the command line.\n");
                return 0;
            }
        }

        // Reads the entire file and puts it into one long malloced string
        entireFile = fileReader(argv[1]);
        
        // Parsing - strtok() splits the strings (ignoring all white spaces), atoi() converts the strings into integers
        // Alphabet -  we have no use for this, so we'll parse it out and ignore
        split = strtok(entireFile, " \n\r\t");
        alphabetNum = atoi(split);
        for (i = 0; i < alphabetNum; i++)
        {
            split = strtok(NULL, " \n\r\t");
        }

        // States - initializes the number of states and inputs all of them into the state list
        split = strtok(NULL, " \n\r\t");
        stateNum = atoi(split);
        state = (char**)malloc((stateNum+1)*sizeof(char*));

        for (i = 0; i < stateNum; i++)
        {
            state[i] = (char*)malloc((STR_MAX+1)*sizeof(char));
            split = strtok(NULL, " \n\r\t");
            strcpy(state[i], split);
        }
        
        // Start state
        split = strtok(NULL, " \n\r\t");
        strcpy(startState, split);
        
        // Accept state
        split = strtok(NULL, " \n\r\t");
        strcpy(acceptState, split);

        // Input Elements - initializes the number of inputs and stores the strings into the input list
        split = strtok(NULL, " \n\r\t");
        inputNum = atoi(split);
        input = (char**)malloc((inputNum+1)*sizeof(char*));

        for (i = 0; i < inputNum; i++)
        {
            input[i] = (char*)malloc((STR_MAX+1)*sizeof(char));
            split = strtok(NULL, " \n\r\t");
            strcpy(input[i], split);
        }

        // Transition List - initializes the number of transitions and stores the strings into the transition list
        split = strtok(NULL, " \n\r\t");
        transitionNum = atoi(split);
        transitionList = (transition*)malloc((transitionNum+1)*sizeof(transition));

        // Gets state 1, input/transition type, and state 2
        for (i = 0; i < transitionNum; i++)
        {
            split = strtok(NULL, " \n\r\t");
            strcpy(transitionList[i].stateOne, split);
            split = strtok(NULL, " \n\r\t");
            strcpy(transitionList[i].transition, split);
            split = strtok(NULL, " \n\r\t");
            strcpy(transitionList[i].stateTwo, split);
        }

        // Initializes the current and next state lists
        currentList = (active*)malloc((stateNum+1)*sizeof(active));
        nextList = (active*)malloc((stateNum+1)*sizeof(active));

        // Sets the current & next state lists to inactive by default
        for (i = 0; i < stateNum; i++)
        {
            strcpy(nextList[i].state, state[i]);
            nextList[i].active = 0;

            strcpy(currentList[i].state, state[i]);

            // If the current state is the start state, activate it
            if (strcmp(currentList[i].state, startState) == 0)
            {
                currentList[i].active = 1;
            }
            else
            {
                currentList[i].active = 0;
            }
        }

        // Checks for any start state epsilons and activates them
        currentList = epsilonList(stateNum, transitionNum, transitionList, currentList);

        // Calculates the next active list by using the current active list
        for (i = 0; i < inputNum; i++)
        {
            printf("\n%s  ", input[i]);

            for (j = 0; j < stateNum; j++)
            {
                for (k = 0; k < transitionNum; k++)
                {
                    // Checks transition list for matching inputs, first states, and active statuses
                    if (strcmp(transitionList[k].transition,input[i]) == 0 && strcmp(transitionList[k].stateOne, currentList[j].state) == 0 && currentList[j].active == 1)
                    {   
                        // If matches are found, activate the second state in that same transition
                        for (x = 0; x < stateNum; x++)
                        {
                            if (strcmp(transitionList[k].stateTwo, nextList[x].state) == 0)
                            {
                                nextList[x].active = 1;
                            }
                        }
                    }
                }
            }
            
            // Checks for any epsilon transitions that come afterwards
            nextList = epsilonList(stateNum, transitionNum, transitionList, nextList);

            for (j = 0; j < stateNum; j++)
            {
                // Prints the next active list
                printf("%d ", nextList[j].active);
                
                // Saves the next active list into the current active list
                currentList[j].active = nextList[j].active;

                // Resets the next active list
                nextList[j].active = 0;
            }
        }

        // Accepts or rejects depending on if the accept state is activated in the last current active list
        for (i = 0; i < stateNum; i++)
        {
            if (currentList[i].active == 1 && strcmp(currentList[i].state,acceptState) == 0)
            {
                printf("\naccept\n\n");
                i = stateNum;
            }
            else if (i == stateNum-1)
            {
                printf("\nreject\n\n");
            }
        }

        // Frees all malloced variables
        free(transitionList);
        for (i = 0; i < inputNum; i++)
        {
            free(input[i]);
        }
        free(input);
        for (i = 0; i < stateNum; i++)
        {
            free(state[i]);
        }
        free(state);
        free(entireFile);
        free(currentList);
        free(nextList);
    }
    else if (argc > 2) 
    {
        // If more than 1 file was given, output this message
        printf("Too many files were given in the command line.\n");
    }
    else
    {
        // If no files were given, output this message
        printf ("No files were given in the command line.");
    }
    return 0;
}

// This function reads an entire file and returns it as one big string
char * fileReader(char fileName[])
{
    // Declaring Variables
    FILE *file;
    char *fileData, fileLine[LINE_MAX];
    int lineNum;
    
    // Initializing Variables
    file = NULL;
    lineNum = 0;

    // Opens the file by using the inputted command line argument
    file = fopen(fileName, "r");

    // Dummy proof: checks if the file exist
    if (file == NULL)
    {
        printf ("%s does not exist.\n", fileName);

        // Exits the program, automatically closes the file
        exit(0);
    }

    // Finds the file size
    fseek(file, 0L, SEEK_END);
    fileData = (char*)malloc((ftell(file)+1)*sizeof(char));

    // Moves to the start of the file
    rewind (file);

    // Puts one row of file text into fileLine
    while (fgets(fileLine, LINE_MAX, file))
    {
        // Removes the newline at the end of each fgets() with a whitespace
        if (fileLine[strlen(fileLine)-1] == '\n')
        {
            fileLine[strlen(fileLine)-1] = ' ';
        }
            
        // Puts the line of file text into fileData
        if (lineNum == 0)
        {
            strcpy(fileData, fileLine);
            lineNum = 1;
        }
        else
        {
            strcat(fileData, fileLine);
        }
    }
        
    // Close the file
    fclose(file);
        
    // Removes the last whitespace in entireFile with NULL
    if (fileData[strlen(fileData)-1] == ' ')
    {
        fileData[strlen(fileData)-1] = '\0';
    }

    // Returns the file contents
    return fileData;
}

// This function updates the current/next list if epsilons are involved
active * epsilonList(int stateNum, int transitionNum, transition *transitionList, active *list)
{
    // Declaring variables
    int i, j, k;
    
    for (i = 0; i < stateNum; i++)
    {
        for (j = 0; j < transitionNum; j++)
        {
            // Checks transition list for matching epsilons, first states, and active statuses
            if (strcmp(transitionList[j].transition,"e") == 0 && strcmp(transitionList[j].stateOne, list[i].state) == 0 && list[i].active == 1)
            {
                for (k = 0; k < stateNum; k++)
                {
                    // Updates the list if a state is active with an epsilon transition
                    if (strcmp(transitionList[j].stateTwo, list[k].state) == 0 && list[k].active == 0)
                    {
                        list[k].active = 1;

                        // Resets the loops to check for any more epsilon activations
                        k = stateNum;
                        j = 0;
                        i = 0;
                    }
                }
            }
        }
    }

    // Returns the list
    return list;
}