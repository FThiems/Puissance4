main: jeu.c
	gcc -Wextra -Wall jeu.c -o main

clean: 
	rm -rf *.o
	rm -rf main
