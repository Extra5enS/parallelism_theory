GCC = gcc
FLAGS = -Wall -c -shared -lm -pthread
NAME = stack.c
OBJ_NAME = stack.o
LIB_NAME = liblfstack.so
RLIB = ranlib

all:
	$(GCC) $(NAME) $(FLAGS) -o $(LIB_NAME)

clean:
	rm -f $(LIB_NAME) $(OBJ_NAME)
