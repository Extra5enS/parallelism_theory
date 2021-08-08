GCC = gcc
FLAGS = -Wall -c -I../include
LF = -L../source -llfstack -I../include
BYN_NAME = demo
OBJ_NAME = s.o
NAME_SINGLE = single_thread_use.c
all: single
	@echo "choose singe or multy thread mode"

single:
	$(GCC) $(NAME_SINGLE) $(FLAGS) -o $(OBJ_NAME)
	$(GCC) $(OBJ_NAME) $(LF)

clean:
	rm -f $(BYN_NAME)
