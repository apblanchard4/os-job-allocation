all : scheduler.c
mem : scheduler.c
	gcc -o scheduler scheduler.c 
