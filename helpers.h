#ifndef HELPERS_H
#define HELPERS_H

typedef struct
{
  char *name;
  struct tm time;
  int ReturnVal;
} Command;


typedef struct
{
  char *name;
  char *value;
} EnvVar;

void colourPickSet(int passedValue);
void printUpperCase();
void colourSelect(char **passedBuffer, int *inputBufferSize, int *colour, Command *com, int indexOfLog);
void printOutput(char **inputBuffer, int *inputBufferSize, EnvVar *createdVariables, int indexOfVariables,  Command *com, int indexOfLog);
char **initializeTokens(char *usersInput, int *inputBufferSize, size_t lineLength);
void addLog(char **inputBuffer, Command *calledCommands, int *indexOfLog);
void addVariable(char **inputBuffer, char *variableName, char *variableValue, EnvVar *createdVariables, int *indexOfVariables);
void variableCreator(char **inputBuffer, int *indexOfEqualsSep, EnvVar *createdVariables, int *indexOfVariables, Command *com, int indexOfLog);
char *getUserInput(char **bufp, size_t *sizep, FILE *fp);
#endif
