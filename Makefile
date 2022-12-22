#compile and link the application
all: cshell

#run the application
run: cshell
	./cshell

#link cshell.o and helper.o to executable main
cshell: cshell.o helpers.o
	gcc -g -o cshell cshell.o helpers.o

#compile the cshell.c to cshell.o
cshell.o: cshell.c
	gcc -g -c cshell.c

#compile the helpers.c to helpers.o
helpers.o: helpers.c
	gcc -g -c helpers.c

#remove built files
clean:
	rm -rf cshell cshell.o helpers.o *~