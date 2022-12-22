#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "helpers.h"

int main(int argc, char *argv[])
{
  size_t lineLength = 50;
  char *userInput = malloc(lineLength * sizeof (char)); // Create userInput array to store all inputs passed in the terminal to this array.
  int colour = 0;       // colour var will be used to keep track of current outputted colour

  size_t calledCommandSize = 256;
  Command *calledCommands = malloc(calledCommandSize*sizeof(Command)); // Create array of Command structs which keep track of called Commands when log is called.
  int indexOfLog = 0;   // Create Int variable to keep track of how many commands were called

  size_t createdVar = 256;
  EnvVar *createdVariables = malloc(createdVar*sizeof(Command));
  int indexOfEqualsSep = 0;
  int indexOfVariables = 0;
  bool isScript = false;

  //Check to see if script is passed in as one of the input arguments
  if (argc > 1)
  {
    FILE *file;
    file = fopen(argv[1], "r");

    if (file == NULL)
    {
      printf("ERROR. File could not be opened. \n");
      return 0;
    }

    isScript = true;
    stdin = file;
  }

  if (isScript == false)
  {
    printf("Welcome. Type in your commands below. \n");
  }

  while(true)
  {
    if (indexOfLog == (calledCommandSize-1))
    {
      calledCommandSize = calledCommandSize*2;
      calledCommands = realloc(calledCommands, calledCommandSize*sizeof(Command));
    }

    // Output should be based on colour chosen by user. Starts off as default
    colourPickSet(colour);

    if (isScript == false)
    {
      printf("cshell$ ");
    }
    colourPickSet(0); // Users input colour should be default
    if (getUserInput(&userInput, &lineLength, stdin) == NULL)
    {
      colourPickSet(colour); // Terminal outputs should be in colour chosen by user.
      printf("Bye!\n");
      colourPickSet(0); // Users input colour should be default
      break;
    }

    // Remove the newline that is added to userInput.
    // Removing newline was found from: https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
    userInput[strcspn(userInput, "\n")] = 0;
    colourPickSet(colour); // Terminal outputs should be in colour chosen by user.

    // If userInput is empty (nothing was passed), ask the user to enter a command.
    if (strcmp(userInput, "") == 0)
    {
      if (isScript == false)
      {
        printf("No command given. Please enter a command. \n");
      }

    }

    else
    {
      char **inputBuffer = NULL;
      int inputBufferSize = 0;
      inputBuffer = initializeTokens(userInput, &inputBufferSize, lineLength);

      // Handle the addition of commands into the calledCommands array.
      addLog(inputBuffer, calledCommands, &indexOfLog);

      // If the first element is exit or Exit, exit the program
      if (strcmp(inputBuffer[0], "Exit") == 0 || strcmp(inputBuffer[0], "exit") == 0)
      {
        if (inputBuffer[1] != NULL)
        {
          printf("Error: Too many arguments passed \n");
          calledCommands[indexOfLog-1].ReturnVal = 1;
          continue;
        }
        free(inputBuffer);
        printf("Bye! \n");
        colourPickSet(0); // Users input colour should be default
        break;
      }

      // If the first element is log or Log, do log stuff
      else if (strcmp(inputBuffer[0], "Log") == 0 || strcmp(inputBuffer[0], "log") == 0)
      {
        //Increment to 1 less to not include current log command
        for (int i = 0; i < indexOfLog - 1; i++)
        {
          printf("%s %s %d \n", asctime(&calledCommands[i].time), calledCommands[i].name, calledCommands[i].ReturnVal);
        }
      }

      // If the first element is theme or Theme, change the colour of the terminal outputs
      // Adding colours to terminal outputs found from: https://www.theurbanpenguin.com/4184-2/
      else if (strcmp(inputBuffer[0], "Theme") == 0 || strcmp(inputBuffer[0], "theme") == 0)
      {
        colourSelect(inputBuffer, &inputBufferSize, &colour, calledCommands, indexOfLog);
      }

      // If the first element is print or Print, print the words that follow
      else if (strcmp(inputBuffer[0], "Print") == 0 || strcmp(inputBuffer[0], "print") == 0)
      {
        printOutput(inputBuffer, &inputBufferSize, createdVariables, indexOfVariables, calledCommands, indexOfLog);
      }

      //**BONUS** If the first element is print or upper, call the system call that was created to uppercase the inputted word
      else if (strcmp(inputBuffer[0], "Uppercase") == 0 || strcmp(inputBuffer[0], "uppercase") == 0)
      {
        if (inputBuffer[1] != NULL)
        {
          for (int index = 1; index < inputBufferSize; index++)
          {
            char temp[sizeof(inputBuffer[index])];
            sprintf(temp, inputBuffer[index]);
            if (syscall(335, temp, sizeof(inputBuffer[index])) != -1)
            {
              printUpperCase();
            }
            else
            {
              printf("System call not implemented! \n");
              calledCommands[indexOfLog - 1].ReturnVal = 1;
              break;
            }
          }
        }
      }

      else if (inputBuffer[0][0] == '$')
      {
        variableCreator(inputBuffer, &indexOfEqualsSep, createdVariables, &indexOfVariables, calledCommands, indexOfLog);
      }

      else
      {
        //Piping example: https://stackoverflow.com/questions/7292642/grabbing-output-from-exec

        int fds[2]; //fd[0] is reading end, fd[1] is writing end
        char output[4096];
        int wstatus;
        int return_value;

        //Check if file descriptors are properly made
        if (pipe(fds) == -1)
        {
          printf("Pipe Failed\n");
          return 1;
        }

        pid_t pid = fork();

        if (pid < 0)
        {
          printf("fork Failed \n");
          return 1;
        }

        //Child process
        else if (pid == 0)
        {
          dup2 (fds[1], STDOUT_FILENO);
          dup2 (fds[1], STDERR_FILENO);

          close(fds[0]);
          close(fds[1]);

          execvp(inputBuffer[0], inputBuffer);
          perror("ERROR");
          _exit(EXIT_FAILURE);
        }

        else
        {
          //Close once done writing all lines (move to where we finish copying commands)
          close(fds[1]);

          waitpid(pid, &wstatus, 0); // Store proc info into wstatus
          int count = read(fds[0], output, sizeof(output));
          printf("%.*s", count, output);
          return_value = WEXITSTATUS(wstatus); // Extract return value from wstatus

          if (return_value == 1)
          {
            calledCommands[indexOfLog-1].ReturnVal = 1;
          }
        }
      }
      free(inputBuffer);
    }
  }

  for (int i = 0; i < indexOfLog; i++)
  {
    free(calledCommands[i].name);
  }

  for (int i = 0; i < indexOfVariables; i++)
  {
    free(createdVariables[i].name);
    free(createdVariables[i].value);
  }

  free(calledCommands);
  free(createdVariables);
  free(userInput);
  return 0;
}