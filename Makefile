main: jeu.c
	gcc -Wextra -Wall jeu.c -o main -lm

clean: 
	rm -rf *.o
	rm -rf main
