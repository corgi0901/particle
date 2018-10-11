TARGET=particle
DOC=doc
CC=g++
CFLAGS=-O2 -Wall -Wextra
DBG_CFLAGS=-g -rdynamic -Wall -Wextra

$(TARGET): *.c *.h
	$(CC) *.c $(CFLAGS) -o $(TARGET)
debug: *.c *.h
	$(CC) *.c $(DBG_CFLAGS) -o $(TARGET)
doc: FORCE
	rm $(doc) -rf; doxygen Doxyfile
test: FORCE
	cd test; ./test-run.sh; cd ../
clean: FORCE
	rm $(TARGET)

FORCE:
.PHONY: FORCE
