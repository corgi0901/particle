TARGET=particle
DOC=doc
CC=g++
CFLAGS=-O2 -Wall -Wextra
DBG_CFLAGS=-g -rdynamic -Wall -Wextra

$(TARGET): *.cpp *.hpp
	$(CC) *.cpp $(CFLAGS) -o $(TARGET)
debug: *.cpp *.hpp
	$(CC) *.cpp $(DBG_CFLAGS) -o $(TARGET)
doc: FORCE
	rm $(doc) -rf; doxygen Doxyfile
test: FORCE
	cd test; ./test-run.sh; cd ../
clean: FORCE
	rm $(TARGET)

FORCE:
.PHONY: FORCE
