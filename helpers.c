#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <fcntl.h>
#include "helpers.h"

void colourPickSet(int passedValue)
{
  if (passedValue == 1)
  {
    printf("\033[1;31m");
  }
  else if (passedValue == 2)
  {
    printf("\033[1;32m");
  }
  else if (passedValue == 3)
  {
    printf("\033[1;34m");
  }
  else if (passedValue == 4)
  {
    printf("\033[1;30m");
  }
  else if (passedValue == 5)
  {
    printf("\033[1;33m");
  }
  else if (passedValue == 6)
  {
    printf("\033[1;36m");
  }
  else if (passedValue == 7)
  {
    printf("\033[1;37m");
  }
  else if (passedValue == 0)
  {
    printf("\033[0m");
  }
}

void colourSelect(char **passedBuffer, int *inputBufferSize, int *colour, Command *com, int indexOfLog)
{
  //Check to see if only one theme is specified
  if (*inputBufferSize == 2)
  {
    if (strcmp(passedBuffer[1], "red") == 0 || strcmp(passedBuffer[1], "Red") == 0)
    {
      *colour = 1;
    }
    else if (strcmp(passedBuffer[1], "green") == 0 || strcmp(passedBuffer[1], "Green") == 0)
    {
      *colour = 2;
    }
    else if (strcmp(passedBuffer[1], "blue") == 0 || strcmp(passedBuffer[1], "Blue") == 0)
    {
      *colour = 3;
    }
    else if (strcmp(passedBuffer[1], "black") == 0 || strcmp(passedBuffer[1], "Black") == 0)
    {
      *colour = 4;
    }
    else if (strcmp(passedBuffer[1], "yellow") == 0 || strcmp(passedBuffer[1], "Yellow") == 0)
    {
      *colour = 5;
    }
    else if (strcmp(passedBuffer[1], "cyan") == 0 || strcmp(passedBuffer[1], "Cyan") == 0)
    {
      *colour = 6;
    }
    else if (strcmp(passedBuffer[1], "white") == 0 || strcmp(passedBuffer[1], "White") == 0)
    {
      *colour = 7;
    }
    else if (strcmp(passedBuffer[1], "reset") == 0 || strcmp(passedBuffer[1], "Reset") == 0)
    {
      *colour = 0;
    }
    else
    {
      printf("Unsupported theme: %s \n", passedBuffer[1]);
      com[indexOfLog - 1].ReturnVal = 1;
    }
  }

  else if (*inputBufferSize > 2)
  {
    printf("Too many arguments passed. \n");
    com[indexOfLog - 1].ReturnVal = 1;
  }

  else
  {
    printf("Theme not specified. \n");
    com[indexOfLog - 1].ReturnVal = 1;
  }
}

/* This function handles printing either the first word after "print" is called,
 or handles the printing of variables.
*/
void printOutput(char **inputBuffer, int *inputBufferSize, EnvVar *createdVariables, int indexOfVariables, Command *com, int indexOfLog)
{
  bool variableFound = false;
  if (inputBuffer[1] != NULL)
  {
    for (int index = 1; index < *inputBufferSize; index++)
    {
      // Check to see if the argument starts with a dollar sign. If it is, print the variables value
      if (inputBuffer[index][0] == '$')
      {
        /*create a char pointer called variableName to store the name of the 1st word from the inputBuffer.
        * According to https://stackoverflow.com/questions/27636306/valgrind-address-is-0-bytes-after-a-block-of-size-8-allocd
        * strcpy adds a null terminator character '\0' which we must allocate memory for (+5)
        * or else we may get memory leaks
        */
        char *variableName = malloc(strlen(inputBuffer[index]) * sizeof(char) + 5);
        strcpy(variableName, inputBuffer[index]);
        char* substr = variableName + 1; // create sub-string without the dollar sign

        // Go through all of the created variables. If this variable already exists, print it out
        // Otherwise, if it doesn't exit, tell the user it hasn't been set.
        for (int i = 0; i < indexOfVariables; i++)
        {
          if (strcmp(createdVariables[i].name, substr) == 0)
          {
            printf("%s ", createdVariables[i].value);
            variableFound = true;
          }
        }
        if (variableFound == false)
        {
          printf("%s ", inputBuffer[index] );
          //printf("%s not set! ", substr);
          //com[indexOfLog - 1].ReturnVal = 1;
        }
        variableFound = false;
        free(variableName);
      }

      else
      {
        printf("%s ", inputBuffer[index] );
      }

    }

    printf("\n");
  }

  // If the user didn't specify what to print, output a warning.
  else
  {
    printf("No argument was provided. \n");
    com[indexOfLog - 1].ReturnVal = 1;
  }
}

char **initializeTokens(char *usersInput, int *inputBufferSize, size_t lineLength)
{
  // With no errors storing the users input, we can tokenize each word to simplify the way we handle commands later on.
  // This was provided as a hint: Write a loop to read a line and split it into tokens"
  // ie: cshells$ print hello my name is James
  // inputBuffer: {print, hello, my, name, is, James}
  // The implementation for tokenizing words was based on of https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
  const char s[2] = " ";             // Each word will be separated by a space
  char *token;                       // Token will hold the current word
  char **inputBuffer = malloc(lineLength*sizeof(char*));               // create new array that will hold every word from userInput, one word per index. Because it's a pointer to a pointer, don't malloc
  token = strtok(usersInput, s);     // update token to be the word based on the separator, s
  int size = *inputBufferSize;       // size is used to keep track of the number of tokens

  while( token != NULL )             // Place each word from userInput into each index of inputBuffer
  {
    inputBuffer[size] = token;
    size++;
    token = strtok(NULL, s);
  }

  inputBuffer[size] = NULL;
  *inputBufferSize = size;
  return inputBuffer;
}

// This function handles adding a struct to the calledCommands array of structs.
void addLog(char **inputBuffer, Command *calledCommands, int *indexOfLog)
{
  // Handle the addition of commands into the calledCommands array.
  // 1) Create temporary command structure
  // 2) Create char pointer which holds the first word of the command from inputbuffer[0]
  // 3) set the temporaryStruct name to be the name of the command passed.
  // 4) set the temporaryStruct time variable
  // 5) set the temporaryStruct ReturnVal to be zero
  // 6) insert the temporary structure in the appropriate index of calledCommands

  Command temporaryStruct;
  char *nameOfCommand = malloc(strlen(inputBuffer[0]) * sizeof(char) + 5);
  strcpy(nameOfCommand, inputBuffer[0]);
  temporaryStruct.name = nameOfCommand;

  // Time found from https://en.wikipedia.org/wiki/C_date_and_time_functions
  time_t current_time = time(NULL);
  temporaryStruct.time = *localtime(&current_time);

  temporaryStruct.ReturnVal = 0;
  int size = *indexOfLog;
  calledCommands[size] = temporaryStruct;
  size++;
  *indexOfLog = size;
}

// This function handles adding a struct to the EnvVar array of structs.
void addVariable(char **inputBuffer, char *variableName, char *variableValue, EnvVar *createdVariables, int *indexOfVariables)
{
  // Handle the addition of variables into the createdVariables array.
  // 1) Create temporary EnvVar structure and set it's variables to NULL.
  // 2) Create boolean to see if the variable is already somewhere in the createdVariables array
  // 3) Go through the current createdVariables array index by index.
  //    If the variable already exists, replace the current variableValue with the new one using memcpy
  // 4) if the variable doesn't exist already, malloc two char pointers for the name and value
  // 5) strcpy both the "variableName" and "variableValue" from the parameters to those char pointers
  // 6) Update the temporaryStruct member variables, and place it into the current index of the createdVariables array
  // 7) update indexOfVariables


  EnvVar temporaryStruct;
  temporaryStruct.name = NULL;
  temporaryStruct.value = NULL;
  bool varExists = false;
  int size = *indexOfVariables;

  for (int i = 0; i < size; i++)
  {
    if (strcmp(createdVariables[i].name, variableName) == 0)
    {
      memcpy (createdVariables[i].value, variableValue, strlen(variableValue) + 1);
      varExists = true;
      break;
    }
  }

  if (varExists == false)
  {
    char *variableNamePassed = malloc(strlen(variableName) * sizeof(char) + 5);
    strcpy(variableNamePassed, variableName);

    char *variableValuePassed = malloc(strlen(variableValue) * sizeof(char) + 5);
    strcpy(variableValuePassed, variableValue);

    temporaryStruct.name = variableNamePassed;
    temporaryStruct.value = variableValuePassed;

    createdVariables[size] = temporaryStruct;
    size++;
    *indexOfVariables = size;
  }
}

void variableCreator(char **inputBuffer, int *indexOfEqualsSep, EnvVar *createdVariables, int *indexOfVariables, Command *com, int indexOfLog)
{
  // If the variable was set in proper syntax (ie, $var=value)
  //if (inputBuffer[1] == NULL)
  //{ //printf("Assigning variables time baybee \n");
  char variableName[1028];
  char variableValue[1028];
  memset(variableName, 0, sizeof variableName);
  memset(variableValue, 0, sizeof variableValue);
  *indexOfEqualsSep = 0;
  // loop over input buffer and find '=' sign
  for(int i = 1; i < 1028; i++)
  {
    //Check for any NULL character that might appear before = sign
    if (inputBuffer[0][i] == '\0'){
      break;
    }
    // equal sign
    if (inputBuffer[0][i] == '=')
    {
      *indexOfEqualsSep = i;
      break;
    }

    else
    {
      // copy variable name
      variableName[i-1] = inputBuffer[0][i];
    }
  }

  //Case for space after ($var= foo)
  if (inputBuffer[0][*indexOfEqualsSep+1] == '\0'
  //Case no variable name or no equal sign ($var OR $=); works also for the case of $var =foo and $var = foo
    || *indexOfEqualsSep == 0)
  {
    printf("Could not discern value in %s. Variable not set. \n", inputBuffer[0]);
    com[indexOfLog - 1].ReturnVal = 1;
    return;
  }

  int j = 0;
  int i = *indexOfEqualsSep +1;
  while(i < 1027 && !(inputBuffer[0][i] == '\0' && inputBuffer[0][i+1] == '\0'))
  {
    if (inputBuffer[0][i] == '\0')
    {
      variableValue[j] = ' ';
    }

    else
    {
      variableValue[j] = inputBuffer[0][i];
    }

    ++i;
    ++j;
  }

  addVariable(inputBuffer, variableName, variableValue, createdVariables, indexOfVariables);

}

// This function was taken and based off of https://stackoverflow.com/questions/37124003/dynamic-buffer-fgets-in-c
char *getUserInput(char **bufp, size_t *sizep, FILE *fp)
{
  if (fgets(*bufp, *sizep, fp) != NULL)
  {
    size_t len = strlen(*bufp);
    while(strchr(*bufp, '\n') == NULL)
    {
      *sizep += 100;
      *bufp = realloc(*bufp, *sizep);
      if (*bufp == NULL)
      {
        printf("Fatal: failed to allocate %zu bytes.\n", *sizep);
        exit(1);
      }
      if(fgets(*bufp + len, *sizep - len, fp) == NULL) return *bufp;
      len += strlen(*bufp + len);
    }
    return *bufp;
  }

  else
  {
    return NULL;
  }
}

// This function is used to output the result of the kern.log path
void printUpperCase()
{
  FILE *fp;
  char path[1035];
  fp = popen("sed -n '$p' /var/log/kern.log", "r");
  if (fp == NULL)
  {
      printf("Failed to run command\n");
      exit(1);
  }
  // print out the output of this line.
  while (fgets(path, sizeof(path), fp) != NULL)
  {
      for (int i = 0; i < strlen(path); i++)
      {
          printf("%c", path[i]);
      }
  }
  pclose(fp);
}
