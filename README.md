# Custom C Shell Project

Hello!

In this project, we have developed a command-line interpreter (or shell) which supports environment variables and history of executed commands. Our shell is capable of:
- Recognizing lines of the form $<VAR>=<value>
- Recognizing lines of the form <command> <arg0> <arg1> … <argN>, where <command> is a name of built-in command.

The following built-in commands are supported:
- exit, the shell terminates on this Command
- log, the shell prints history of executed commands with time and return code
- print, the shell prints argument given to this Command
- theme, the shell changes the color of and output

## Running the file: 
To compile this program, run:
"make"

To run the file in the standard interactive mode, type:
./cshell

To run the file in script mode (using the file "script.txt"), type:
./cshell script.txt
- script.txt can be modified to fit your needs
