GCC = gcc
FLAGS = -Wall -c -I../include -pthread
LF = -L../source -llfstack -I../include -pthread
BYN_NAME = demo
S_OBJ_NAME = s.o
M_OBJ_NAME = m.o
NAME_SINGLE = single_thread_use.c
NAME_MULTY = multy_thread_use.c
all: single multy
	@echo "choose singe or multy thread mode"

single:
	$(GCC) $(NAME_SINGLE) $(FLAGS) -o $(S_OBJ_NAME)
	$(GCC) $(S_OBJ_NAME) $(LF) -o singletest.out

multy:
	$(GCC) $(NAME_MULTY) $(FLAGS) -o $(M_OBJ_NAME)
	$(GCC) $(M_OBJ_NAME) $(LF) -o multytest.out
	

main:
	make -C ./.. all

clean:
	rm -f $(BYN_NAME) *.o *.out
