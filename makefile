NAME=os_ass2

CC=gcc
FLAGS=-Wall -Wextra

RM =rm -rf


main: main.c echo.c prompt.c pwd.c cd.c ls.c discover.c pinfo.c history.c jobs.c
	$(CC) $(FLAGS) main.c echo.c prompt.c pwd.c cd.c ls.c discover.c pinfo.c history.c jobs.c sig.c bg.c fg.c -o shell


