all: single multy
	@echo "'-_-"

lib:
	make -C ./source/

single: lib
	make -C ./demo/ single

multy: lib
	make -C ./demo/ multy

clean:
	make -C ./source clean
	make -C ./demo clean
	
